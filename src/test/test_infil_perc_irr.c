/* headers used in the tests */
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lpj.h"

/* modules under test */
#include "infil_perc_irr.h"


/* mocked modules d*/
#include "mock_infil_stubs.h"
#include "mock_soiltemp_stubs.h"

/* global lpjml variables */
Real soildepth[] = {200.0,300.0,500.0,1000.0,1000.0,10000.0};
Config aconfig = {.percolation_heattransfer=1};
Param param = { .soil_infil=6, .percthres =1, .frac_ro_stored =0.8,  .rw_buffer_max=20 };

/* helpers for the tests */
#include "soil_therm_testing_helper.c" 
#include "soil_freeze_frac_testing_helper.c"
#include "apply_perc_enthalpy.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "calc_soil_thermal_props.h" 
#include "enth2freezefrac.h"
#include "freezefrac2soil.h"
#include "update_soil_thermal_state.h"
#include "apply_enth_of_untracked_mass_shifts.h"
#include "apply_heatconduction_of_a_day.h"

void setUp (void) {
        aconfig.johansen=TRUE;
} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */



void theTempsShouldHaveRemainedEqual(Real * tempsNow, Real startTemp);




void test_soiltempsShouldNotChangeIfDripIrrInfilHasSameTemperature(void){
    Stand * st = setupDefaultStand();
    Real return_flow_b =0;
    Real startSoilTemp= 5.0;
    Real freezeFrac[NSOILLAYER];
    Soil_thermal_prop th;
    Irrigation * irr = st->data;
    irr->irrig_system = DRIP;

    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    for (int i = 0; i < 10; i++)
    {
        infil_perc_irr(st,20,startSoilTemp*c_water+c_water2ice,&return_flow_b,1,1,&aconfig);     
        apply_perc_enthalpy(&st->soil);
    }
    updateMeanLayerTemps(&st->soil);

    TEST_ASSERT_EACH_EQUAL_DOUBLE(startSoilTemp, st->soil.temp, NSOILLAYER);
    teardownStand(st);
}


void test_soiltempShouldNotChangeIfDripIrrInfilHasSameTemperatureSoilA(void){
    Stand * st = setupDefaultStand();
    Real return_flow_b =0;
    Real startSoilTemp= 5.0;
    Real freezeFrac[NSOILLAYER];
    Soil_thermal_prop th;
    Irrigation * irr = st->data;
    irr->irrig_system = DRIP;
    setup_soil_A(&st->soil);

    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    for (int i = 0; i < 10; i++)
    {
        infil_perc_irr(st,20,startSoilTemp*c_water+c_water2ice,&return_flow_b,1,1,&aconfig);     
        apply_perc_enthalpy(&st->soil);
    }
    updateMeanLayerTemps(&st->soil);

    TEST_ASSERT_EACH_EQUAL_DOUBLE(startSoilTemp, st->soil.temp, NSOILLAYER);
    teardownStand(st);
}
    


void test_soiltempsShouldNotChangeIfSprinklerIrrInfilHasSameTemperature(void){
    Stand * st = setupDefaultStand();
    Real return_flow_b =0;
    Real startSoilTemp= 5.0;
    Real freezeFrac[NSOILLAYER];
    Soil_thermal_prop th;
    Real waterdiff[NSOILLAYER];
    Real soliddiff[NSOILLAYER];

    Irrigation * irr = st->data;
    irr->irrig_system = SPRINK;
    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    for (int i = 0; i < 10; i++)
    {
        infil_perc_irr(st,20,startSoilTemp*c_water+c_water2ice,&return_flow_b,1,1,&aconfig);     
        apply_perc_enthalpy(&st->soil);
    }
    updateMeanLayerTemps(&st->soil);
    get_unaccounted_changes_in_water_and_solids(waterdiff,soliddiff,&st->soil);
    TEST_ASSERT_EACH_EQUAL_DOUBLE(startSoilTemp, st->soil.temp, NSOILLAYER);
    teardownStand(st);
}

void test_soiltempsShouldNotChangeIfSprinklerIrrInfilHasSameTemperatureSoilA(void){
    
    Stand * st = setupDefaultStand();
    Real return_flow_b =0;
    Real startSoilTemp= 5.0;
    Real freezeFrac[NSOILLAYER];
    Soil_thermal_prop th;
    Real waterdiff[NSOILLAYER];
    Real soliddiff[NSOILLAYER];
    setup_soil_A(&st->soil);    

    Irrigation * irr = st->data;
    irr->irrig_system = SPRINK;
    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    updateSoilVariables(&st->soil);
    for (int i = 0; i < 10; i++)
    {
        infil_perc_irr(st,20,startSoilTemp*c_water+c_water2ice,&return_flow_b,1,1,&aconfig);     
        apply_perc_enthalpy(&st->soil);
    }
    updateMeanLayerTemps(&st->soil);
    
    get_unaccounted_changes_in_water_and_solids(waterdiff,soliddiff,&st->soil);
    printf("waterdiff: %f, %f, %f, %f, %f, %f\n",waterdiff[0],waterdiff[1],waterdiff[2],waterdiff[3],waterdiff[4],waterdiff[5]);
    UNITY_TEST_ASSERT_DOUBLE_WITHIN(0.001, 0, waterdiff[0], 1, NULL);
    printf("soil water content: %f, %f, %f, %f, %f, %f\n",st->soil.w[0],st->soil.w[1],st->soil.w[2],st->soil.w[3],st->soil.w[4],st->soil.w[5]);
    TEST_ASSERT_EACH_EQUAL_DOUBLE(startSoilTemp, st->soil.temp, NSOILLAYER);
    teardownStand(st);
}


