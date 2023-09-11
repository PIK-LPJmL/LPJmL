/* headers used in the tests */
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lpj.h"

/* modules under test */
#include "infil_perc_irr.h"
#include "update_soil_thermal_state.h"

#include "apply_perc_enthalpy.h"
#include "apply_enth_of_untracked_mass_shifts.h"
#include "apply_heatconduction_of_a_day.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "calc_soil_thermal_props.h" 
#include "enth2freezefrac.h"
#include "freezefrac2soil.h"

/* mocked modules */
#include "mock_infil_stubs.h"
#include "mock_soiltemp_stubs.h"


/* global lpjml variables */
Real soildepth[] = {200.0,300.0,500.0,1000.0,1000.0,10000.0};
Config aconfig = {.percolation_heattransfer=1};
Param param = { .soil_infil=6, .percthres =1, .frac_ro_stored =0.8,  .rw_buffer_max=20 };

/* helpers for the tests */
#include "soil_therm_testing_helper.c" 


void setUp (void) {
        aconfig.johansen=TRUE;
} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */


void test_soiltempsShouldNotChangeInEquilibriumUnderSoiltempAndInfilDrip(void){
    Stand * st = setupDefaultStand();
    Real return_flow_b =0;
    Real startSoilTemp= 5.0;
    Real freezeFrac[NSOILLAYER];
    Soil_thermal_prop th;

    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    for (int i = 0; i < 10; i++)
    {
        update_soil_thermal_state(&st->soil,startSoilTemp, &aconfig);
        infil_perc_irr(st,20,startSoilTemp*c_water+c_water2ice,&return_flow_b,1,1,&aconfig);     
    }

    theTempsShouldHaveRemainedEqual(st->soil.temp, startSoilTemp);
    teardownStand(st);
}

void test_soiltempsShouldNotChangeInEquilibriumUnderSoiltempAndInfilDripSoilB(void){
    Stand * st = setupSoilBStand();
    Real return_flow_b =0;
    Real startSoilTemp= 5.0;
    Real freezeFrac[NSOILLAYER];
    Soil_thermal_prop th;

    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    for (int i = 0; i < 10; i++)
    {
        update_soil_thermal_state(&st->soil,startSoilTemp, &aconfig);
        infil_perc_irr(st,20,startSoilTemp*c_water+c_water2ice,&return_flow_b,1,1,&aconfig);     
    }

    theTempsShouldHaveRemainedEqual(st->soil.temp, startSoilTemp);
    teardownStand(st);
}

void test_soiltempsShouldNotChangeInEquilibriumUnderSoiltempAndInfilSprink(void){
    Stand * st = setupDefaultStand();
    Real return_flow_b =0;
    Real startSoilTemp= 5.0;
    Real freezeFrac[NSOILLAYER];
    Soil_thermal_prop th;
    Irrigation * irr = st->data;
    irr->irrig_system = SPRINK;

    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    for (int i = 0; i < 10; i++)
    {
        update_soil_thermal_state(&st->soil,startSoilTemp, &aconfig);
        infil_perc_irr(st,20,startSoilTemp*c_water+c_water2ice,&return_flow_b,1,1,&aconfig);     
    }

    theTempsShouldHaveRemainedEqual(st->soil.temp, startSoilTemp);
    teardownStand(st);
}

void test_soiltempsShouldNotChangeInEquilibriumUnderSoiltempAndInfilSprinkSoilB(void){
    Stand * st = setupSoilBStand();
    Real return_flow_b =0;
    Real startSoilTemp= 5.0;
    Real freezeFrac[NSOILLAYER];
    Soil_thermal_prop th;
    Irrigation * irr = st->data;
    irr->irrig_system = SPRINK;

    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    for (int i = 0; i < 10; i++)
    {
        update_soil_thermal_state(&st->soil,startSoilTemp, &aconfig);
        infil_perc_irr(st,20,startSoilTemp*c_water+c_water2ice,&return_flow_b,1,1,&aconfig);     
    }

    theTempsShouldHaveRemainedEqual(st->soil.temp, startSoilTemp);
    teardownStand(st);
}



