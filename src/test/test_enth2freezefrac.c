/* headers used in the tests */
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lpj.h"

/* modules under test */
#include "enth2freezefrac.h"

/* mocked modules */

/* global lpjml variables */
Real soildepth[] = {200.0,300.0,500.0,1000.0,1000.0,10000.0};
Config aconfig = {};

/* helpers for the tests */
#include "calc_soil_thermal_props.h" 
#include "freezefrac2soil.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "soil_therm_testing_helper.c" 
#include "soil_freeze_frac_testing_helper.c"
void calculateFreezeFractions(Real *, Soil *);



void setUp (void) {
    /* Setup a soil */
    aconfig.johansen =TRUE;
} /* Is run before every test, put unit init calls here. */
void tearDown (void) {
} /* Is run after every test, put unit clean-up calls here. */


void test_freezefracShouldBeZeroIfSoilContainsFullLatentHeat(){
    Soil defaultSoil;
    Real freezeFrac[NSOILLAYER];
    setup_default_soil(&defaultSoil);

    setEnthalpiesToLatentHeat(&defaultSoil);
    calculateFreezeFractions(freezeFrac, &defaultSoil);

    TEST_ASSERT_EACH_EQUAL_DOUBLE(0,freezeFrac,NSOILLAYER);
}


void test_freezefracShouldBeOneIfSoilContainsNoLatentHeat(){
    Soil defaultSoil;
    Real freezeFrac[NSOILLAYER];
    setup_default_soil(&defaultSoil);

    setEnthalpiesToZero(&defaultSoil);
    calculateFreezeFractions(freezeFrac, &defaultSoil);

    TEST_ASSERT_EACH_EQUAL_DOUBLE(1,freezeFrac,NSOILLAYER);
}



void test_freezefracShouldBeHalfIfSoilContainsHalfLatentHeat(){
    Soil defaultSoil;
    Real freezeFrac[NSOILLAYER];
    setup_default_soil(&defaultSoil);

    setEnthalpiesToHalfLatentHeat(&defaultSoil);
    calculateFreezeFractions(freezeFrac, &defaultSoil);

    TEST_ASSERT_EACH_EQUAL_DOUBLE(0.5,freezeFrac,NSOILLAYER);
}

void test_freezeFracShouldBeOneIfSoilHasNoWaterNoPoresAndZeroEnth(void){
    Soil soil;
    Real freezeFrac[NSOILLAYER];
    setup_soil_A(&soil);
    int l;

    foreachsoillayer(l){
        soil.wsats[l] = 0;
        soil.wsat[l] = 0;
        /*set all water variables to zero*/
        soil.w[l] = 0;
        soil.w_fw[l] = 0;
        soil.wpwp[l] = 0;
        soil.wpwps[l] = 0;
        soil.whcs[l] = 0;
        soil.whc[l] = 0;
        soil.ice_pwp[l] = 0;
        soil.ice_fw[l] = 0;
        soil.ice_depth[l] = 0;
    }
    
    /*set entalpies to zero */
    foreachsoillayer(l){
        for(int i = 0; i < GPLHEAT; i++)
            soil.enth[l*GPLHEAT+i] = 0;
    }
    calculateFreezeFractions(freezeFrac, &soil);

    TEST_ASSERT_EACH_EQUAL_DOUBLE(1,freezeFrac,NSOILLAYER);

}


void test_freezeFracShouldBeZeroIfSoilHasNoWaterNoPoresAndPositiveEnth(void){
    Soil soil;
    Real freezeFrac[NSOILLAYER];
    setup_soil_A(&soil);
    int l;

    foreachsoillayer(l){
        soil.wsats[l] = 0;
        soil.wsat[l] = 0;
        /*set all water variables to zero*/
        soil.w[l] = 0;
        soil.w_fw[l] = 0;
        soil.wpwp[l] = 0;
        soil.wpwps[l] = 0;
        soil.whcs[l] = 0;
        soil.whc[l] = 0;
        soil.ice_pwp[l] = 0;
        soil.ice_fw[l] = 0;
        soil.ice_depth[l] = 0;
    }
    
    /*set entalpies to positive number */
    foreachsoillayer(l){
        for(int i = 0; i < GPLHEAT; i++)
            soil.enth[l*GPLHEAT+i] = 0;
    }
    calculateFreezeFractions(freezeFrac, &soil);

    TEST_ASSERT_EACH_EQUAL_DOUBLE(1,freezeFrac,NSOILLAYER);

}




