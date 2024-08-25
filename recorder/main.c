#include <stdio.h>

#ifdef _WIN32
#include <Windows.h>
#endif

#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <fobos/fobos.h>
#include <wav/wav_file.h>

//==============================================================================
typedef struct rx_ctx_t
{
    struct fobos_dev_t * dev;
    struct wav_file_t * wav;
    int buff_count;
} rx_ctx_t;
//==============================================================================
void read_samples_callback(float *buf, uint32_t buf_length, void *ctx)
{
    struct rx_ctx_t * rx_ctx = (struct rx_ctx_t *)ctx;
    rx_ctx->buff_count++;

    if (rx_ctx->buff_count == 2)
    {
        //printf("\r\n");
        //print_buff(buf, size);
        //print_buff(buf, 1024);
        //print_buff(buf, 64);
        //printf("\r\n");
    }
    //printf("%d ", buf_length);
    printf("+");

    int max_count = 1024;
    max_count = 2048;
    //max_count = 512;
    //max_count = 256;
    //max_count = 64;
    //max_count = 16;

    if (rx_ctx->buff_count == max_count)
    {
        printf("canceling...");
        fobos_rx_cancel_async(rx_ctx->dev);
    }

    if (rx_ctx->wav)
    {
        wav_file_write_data(rx_ctx->wav, (void*)buf, buf_length * 2 * sizeof(float));
        wav_file_write_header(rx_ctx->wav);
    }
}
//==============================================================================
//==============================================================================
void test_rx(int mode)
{
    struct fobos_dev_t * dev = NULL;
    int result = 0;
    char lib_version[32];
    char drv_version[32];
    char serials[256] = {0};

    int index = 0;

    char hw_revision[32];
    char fw_version[32];
    char manufacturer[32];
    char product[32];
    char serial[32];

    printf("Fobos SDR API test applications\n");

    fobos_rx_get_api_info(lib_version, drv_version);

    printf("API Info lib: %s drv: %s\n", lib_version, drv_version);

    int count = fobos_rx_get_device_count(); // just gets devices count

    printf("found devices: %d\n", count);

    count = fobos_rx_list_devices(serials); // enumerates all connected devices with serial numbers

    if (count > 0)
    {
        char * pserial = strtok(serials, " ");
        for(int i = 0; i < count; i++)
        {
            printf("   sn: %s\n", pserial);
            pserial = strtok(0, " ");
        }

        result = fobos_rx_open(&dev, index);

        if (result == 0)
        {
            result = fobos_rx_get_board_info(dev, hw_revision, fw_version, manufacturer, product, serial);
            if (result != 0)
            {
                printf("fobos_rx_get_board_info - error!\n");
            }
            else
            {
                printf("board info\n");
                printf("    hw_revision:  %s\n", hw_revision);
                printf("    fw_version:   %s\n", fw_version);
                printf("    manufacturer: %s\n", manufacturer);
                printf("    product:      %s\n", product);
                printf("    serial:       %s\n", serial);
            }            

            switch (mode)
            {
                case 0:
                {
                    printf("test: fobos_rx_set_user_gpo\n");
                    int usr_gpo = 0;
                    while (1)
                    {
                        result = fobos_rx_set_user_gpo(dev, usr_gpo);
                        if (result != 0)
                        {
                            printf("fobos_rx_set_user_gpo error!\n");
                            break;
                        }
                        usr_gpo++;
                        usr_gpo %= 16;
                        printf(".");
                        //Sleep(1000);
                    }
                    break;
                }
                case 1:
                {
                    printf("test: streaming\n");
                    double frequency = 100000000.0;   // Hz
                    double frequency_actual = 0.0;
                    double samplerate = 25000000.0;   // Hz aka samples per second
                    double samplerate_actual = 0.0;
                    unsigned int direct_sampling = 0; // 0 - RF, 1 - HF1+HF2
                    unsigned int lna_gain = 0;        // ow noise amplifier 0..2
                    unsigned int vga_gain = 0;        // variable gain amplifier 0..15
                    unsigned int clk_source = 0;      // 0 - internal clock (default); 1 - external

                    result = fobos_rx_set_frequency(dev, frequency, &frequency_actual);
                    if (result != 0)
                    {
                        printf("fobos_rx_set_frequency - error!\n");
                    }
                    else
                    {
                        printf("actual frequecy = %f\n", frequency_actual);
                    }

                    result = fobos_rx_set_direct_sampling(dev, direct_sampling);
                    if (result != 0)
                    {
                        printf("fobos_rx_set_direct_sampling - error!\n");
                    }

                    result = fobos_rx_set_lna_gain(dev, lna_gain);
                    if (result != 0)
                    {
                        printf("fobos_rx_set_lna_gain - error!\n");
                    }

                    result = fobos_rx_set_vga_gain(dev, vga_gain);
                    if (result != 0)
                    {
                        printf("fobos_rx_set_vga_gain - error!\n");
                    }

                    result = fobos_rx_set_samplerate(dev, samplerate, &samplerate_actual);
                    if (result != 0)
                    {
                        printf("fobos_rx_set_samplerate - error!\n");
                    }
                    else
                    {
                        printf("actual samplerate = %f\n", samplerate_actual);
                    }

                    result = fobos_rx_set_clk_source(dev, clk_source); 
                    if (result != 0)
                    {
                        printf("fobos_rx_set_samplerate - error!\n");
                    }

                    struct rx_ctx_t rx_ctx;
                    rx_ctx.buff_count = 0;
                    rx_ctx.dev = dev;

                    struct wav_file_t * wav = wav_file_create();
                    wav->channels_count = 2;
                    wav->sample_rate = (int)samplerate;
                    wav->bytes_per_sample = 4;
                    wav->audio_format = 3;

                    const char * file_name = "rx.iq.wav";

                    result = wav_file_open(wav, file_name, "w");
                    if (result != 0)
                    {
                        printf("could not create file %s\n", file_name);
                    }

                    rx_ctx.wav = wav;

                    result = fobos_rx_read_async(dev, read_samples_callback, &rx_ctx, 16, 65536);
                    if (result == 0)
                    {
                        printf("fobos_rx_read_async - ok!\n");
                    }
                    else
                    {
                        printf("fobos_rx_read_async - error!\n");
                    }

                    wav_file_destroy(wav);
                    wav = 0;

                    break;
                }
                default:
                    break;
            }

            fobos_rx_close(dev);
        }
        else
        {
            printf("could not open device! err (%i)\n", result);
        }

    }
    else
    {

    }
}
//==============================================================================
int main(int argc, char** argv)
{
    printf("fobos ast test app\n");
    for(int i = 0; i < argc; i++)
    {
        printf("arg[%d]=%s\n", i, argv[i]);
    }
    printf("machine: x%ld\n", sizeof(void*)*8);
    int mode = 0;
    mode = 1;

    test_rx(mode);

#ifdef _WIN32
    system("pause");
#endif

    return 0;
}
//==============================================================================
