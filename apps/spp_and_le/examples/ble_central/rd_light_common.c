// #include "system/app_core.h"
#include "system/includes.h"
#include "rd_light_common.h"
#include  "K9B_remote.h"

const uint16_t DIM_LUMEN_TO_PWM[101] = {
    0,     1,     4,    9,    16,    25,    36,    49,    64,    81,
   100,   121,   144,   169,   196,   225,   256,   289,   324,   361,
   400,   441,   484,   529,   576,   625,   676,   729,   784,   841,
   900,   961,  1024,  1089,  1156,  1225,  1296,  1369,  1444,  1521,
  1600,  1681,  1764,  1849,  1936,  2025,  2116,  2209,  2304,  2401,
  2500,  2601,  2704,  2809,  2916,  3025,  3136,  3249,  3364,  3481,
  3600,  3721,  3844,  3969,  4096,  4225,  4356,  4489,  4624,  4761,
  4900,  5041,  5184,  5329,  5476,  5625,  5776,  5929,  6084,  6241,
  6400,  6561,  6724,  6889,  7056,  7225,  7396,  7569,  7744,  7921,
  8100,  8281,  8464,  8649,  8836,  9025,  9216,  9409,  9604,  9801,
 10000
};
volatile  rd_light_ctrl_t rd_light_ctrl_val = {0};
static void rd_gpio_init(void);


void rd_light_init(void){
    rd_K9B_flash_init(); 
    rd_gpio_init();   

    printf("Rang Dong DownLight DM BLE V%02d.%02d.01", FIRMWARE_VER_H, FIRMWARE_VER_L);
}

static void rd_gpio_init(void){
  timer_pwm_init(DIM_PWM_TIMER, DIM_PIN, PWM_HZ, 0); //1KHz 50%
  timer_pwm_init(CCT_PWM_TIMER, CCT_PIN, PWM_HZ, 0); //1KHz 50%
  
}


static inline uint16_t dim_cct_2_pwm(uint8_t dim_cct100){
    dim_cct100 = LIMIT_RANGE(dim_cct100, 0, 100);
    return (dim_cct100*100);
}

static inline uint16_t dim_mapping_2_pwm(uint8_t dim_100){
    dim_100 = LIMIT_RANGE(dim_100, 0, 100);
    return (DIM_LUMEN_TO_PWM[dim_100]);
}
/**
 * set dim 100 
 */
void rd_light_set_dim100(uint8_t dim100_set){
    dim100_set = LIMIT_RANGE(dim100_set, 0, 100);
    rd_light_ctrl_val.dim_target = dim100_set;
}

void rd_light_set_cct100(uint8_t cct100_set){
    cct100_set = LIMIT_RANGE(cct100_set, 0, 100);
    rd_light_ctrl_val.cct_target = cct100_set;
}

uint16_t rd_light_get_dim100(void){
    return rd_light_ctrl_val.dim_present;
}

uint16_t rd_light_get_cct100(void){
    return rd_light_ctrl_val.cct_present;
}

void rd_light_set_dim_cct100(uint8_t dim100_set, uint8_t cct100_set){
    dim100_set = LIMIT_RANGE(dim100_set, 0, 100);
    cct100_set = LIMIT_RANGE(cct100_set, 0, 100);
    rd_light_ctrl_val.dim_target = dim100_set;
    rd_light_ctrl_val.cct_target = cct100_set;
}

void rd_light_check_ctrl_pwm(void){
    const uint8_t DIM_STEP = 1; // %
    const uint8_t CCT_STEP  = 1; // %
    
    // check cct pwm
    if(rd_light_ctrl_val.cct_present != rd_light_ctrl_val.cct_target){
        if(rd_light_ctrl_val.cct_present < rd_light_ctrl_val.cct_target){
            rd_light_ctrl_val.cct_present += CCT_STEP;
        }
        if(rd_light_ctrl_val.cct_present > rd_light_ctrl_val.cct_target){
            rd_light_ctrl_val.cct_present -= CCT_STEP;
        }
        set_timer_pwm_duty(CCT_PWM_TIMER, dim_cct_2_pwm(rd_light_ctrl_val.cct_present));
       // set_timer_pwm_duty(JL_TIMER2, dim_stt_last/2);

    }

    // check dim pwm
    if(rd_light_ctrl_val.dim_present != rd_light_ctrl_val.dim_target){
        if(rd_light_ctrl_val.dim_present < rd_light_ctrl_val.dim_target){
            rd_light_ctrl_val.dim_present += DIM_STEP;
        }
        if(rd_light_ctrl_val.dim_present > rd_light_ctrl_val.dim_target){
            rd_light_ctrl_val.dim_present -= DIM_STEP;
        }
        set_timer_pwm_duty(DIM_PWM_TIMER, dim_cct_2_pwm(rd_light_ctrl_val.dim_present));
        //set_timer_pwm_duty(DIM_PWM_TIMER, dim_mapping_2_pwm(rd_light_ctrl_val.dim_present));

       // set_timer_pwm_duty(JL_TIMER2, dim_stt_last/2);
    }

}