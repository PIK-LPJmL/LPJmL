/* headers used in the tests */
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lpj.h"

/* modules under test */
#include "infil_perc_rain.h"


/* mocked modules */
#include "mock_infil_stubs.h"
#include "mock_soiltemp_stubs.h"

/* global lpjml variables */
Real soildepth[] = {200.0,300.0,500.0,1000.0,1000.0,10000.0};
Config aconfig = {.percolation_heattransfer =1};
Param param = { .soil_infil=6, .percthres =1, .frac_ro_stored =0.8,  .rw_buffer_max=20 };

/* helpers for the tests */
#include "soil_therm_testing_helper.c" 
#include "apply_perc_enthalpy.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "calc_soil_thermal_props.h" 
#include "enth2freezefrac.h"
#include "freezefrac2soil.h"
#include "update_soil_thermal_state.h"
#include "apply_enth_of_untracked_mass_shifts.h"
#include "apply_heatconduction_of_a_day.h"
#include "soil_freeze_frac_testing_helper.c"

void setUp (void) {
        aconfig.johansen=TRUE;
} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */



void theTempsShouldHaveRemainedEqual(Real * tempsNow, Real startTemp);
void setTopLayerEnthalpyToHalfLatentHeat(Soil * soil);



void test_soiltempsShouldNotChangeIfRainInfilHasSameTemperature(void){
    Stand * st = setupDefaultStand();
    Real return_flow_b =0;
    Real startSoilTemp= 5.0;
    Soil_thermal_prop th;

    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    for (int i = 0; i < 10; i++)
    {
        infil_perc_rain(st,20,startSoilTemp*c_water+c_water2ice,&return_flow_b,1,1,(&aconfig));     
        apply_perc_enthalpy(&st->soil);
    }

    theTempsShouldHaveRemainedEqual(st->soil.temp, startSoilTemp);
    teardownStand(st);
}

void test_soiltempsShouldNotChangeIfRainInfilHasSameTemperatureSoilB(void){
    Stand * st = setupSoilBStand();
    Real return_flow_b =0;
    Real startSoilTemp= 5.0;
    Soil_thermal_prop th;


    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    for (int i = 0; i < 10; i++)
    {
        infil_perc_rain(st,20,startSoilTemp*c_water+c_water2ice,&return_flow_b,1,1,(&aconfig));     
        apply_perc_enthalpy(&st->soil);
    }
    updateMeanLayerTemps(&st->soil);
    theTempsShouldHaveRemainedEqual(st->soil.temp, startSoilTemp);
    teardownStand(st);
}

void test_highEnergyInfluxInFrozenLayerShouldNotChangeTemp(void){
    Stand * st = setupSoilBStand();
    Real return_flow_b =0;
    Real startSoilTemp= -5.0;
    Soil_thermal_prop th;
    Real high_temperature = 20.0;

    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    updateSoilVariables(&st->soil);
    

    for (int i = 0; i < 10; i++)
    {
        infil_perc_rain(st,20,high_temperature*c_water+c_water2ice,&return_flow_b,1,1,(&aconfig));
        apply_perc_enthalpy(&st->soil);
    }
    updateMeanLayerTemps(&st->soil);
    TEST_ASSERT_EQUAL_DOUBLE(startSoilTemp, st->soil.temp[0]);
}

// void test_grunoffFromFreezeDepthChangeShouldNotChangeTemp(void){
//     Stand * st = setupDefaultStand();
//     Real return_flow_b =0;
//     Real startSoilTemp= 0;
//     Soil_thermal_prop th;

//     //print water content of soil
//     outputLiquidWaterContent(&(st->soil));

//     setEnthalpiesToLatentHeat(&st->soil);

//     updateSoilVariables(&st->soil);
    
//     infil_perc_rain(st,50,c_water2ice,&return_flow_b,1,1,(&aconfig));
//     apply_perc_enthalpy(&st->soil);
    

//     setTopLayerEnthalpyToHalfLatentHeat(&st->soil);
//     updateSoilVariables(&st->soil);
    
//     infil_perc_rain(st,50,c_water2ice,&return_flow_b,1,1,(&aconfig));
//     for(int i=0;i<NSOILLAYER;i++)
//         printf("percolation energy: %f\n", st->soil.perc_energy[i]);
//     apply_perc_enthalpy(&st->soil);

//     updateMeanLayerTemps(&st->soil);

//     TEST_ASSERT_EQUAL_DOUBLE(startSoilTemp, st->soil.temp[0]);
// }


void setTopLayerEnthalpyToHalfLatentHeat(Soil * soil){

    Soil_thermal_prop th;
    calc_soil_thermal_props(&th, soil, NULL,NULL,TRUE,FALSE);

    for(int i=0;i<GPLHEAT;i++)
        soil->enth[i]=th.latent_heat[i]/2;
}