#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lpj.h"

/* modules under test */
#include "calc_soil_thermal_props.h" 
#include "compute_mean_layer_temps_from_enth.h"

/* mocked modules */

/* global lpjml variables */
Real soildepth[] = {200.0,300.0,500.0,1000.0,1000.0,10000.0};
Config aconfig = {};

/* helpers for the tests */
#include "soil_therm_testing_helper.c" 
void theResultingMeanLayerTempsShouldEqualTheMeanOriginalTemps(Real * layerTemps, Real  *originalTemps);

void setUp (void) {
    aconfig.johansen =TRUE;
} /* Is run before every test, put unit init calls here. */
void tearDown (void) {
} /* Is run after every test, put unit clean-up calls here. */

void test_soillayertempsShouldBeZeroIfEnthalpyIsZero(){
    Soil soil;
    Real randomTemp;
    setup_default_soil(&soil);

    setEnthalpiesToZero(&soil);
    updateMeanLayerTemps(&soil);

    TEST_ASSERT_EACH_EQUAL_DOUBLE(0,soil.temp, NSOILLAYER);
}

void test_soillayertempsShouldBeZeroIfEnthalpyEqualsLatentHeat(){
    Soil soil;
    Real randomTemp;
    setup_default_soil(&soil);

    setEnthalpiesToLatentHeat(&soil);
    updateMeanLayerTemps(&soil);

    TEST_ASSERT_EACH_EQUAL_DOUBLE(0,soil.temp, NSOILLAYER);
}



void test_soillayertempShouldRecoverIfEnthalpyCorrespondsToGivenRandTemps(){
    Soil soil;
    Real randomTemp=0;
    for (int i = 0; i < 1000; i++)
    {
        setup_default_soil(&soil);

        setEnthalpiesCorrespToRandTemp(&soil, &randomTemp, 50);
        updateMeanLayerTemps(&soil);

        TEST_ASSERT_EACH_EQUAL_DOUBLE(randomTemp ,soil.temp, NSOILLAYER);
    }
}

void test_soillayertempShouldRecoverIfEnthalpyCorrespondsToGivenRandTempsSoilA(){
    Soil soil;
    Real randomTemp=0;
    for (int i = 0; i < 1000; i++)
    {
        setup_soil_A(&soil);

        setEnthalpiesCorrespToRandTemp(&soil, &randomTemp, 50);
        updateMeanLayerTemps(&soil);

        TEST_ASSERT_EACH_EQUAL_DOUBLE(randomTemp ,soil.temp, NSOILLAYER);
    }
}

void test_meanLayertempsShouldRecoverIfEnthalpiesCorrespondToIncreasingTemps(){
    Soil soil;
    Real linearIncreaseTemps[NHEATGRIDP];

    setup_default_soil(&soil);
    setEnthalpiesCorrespondingToLinearIncreaseTemps(&soil, linearIncreaseTemps);
    updateMeanLayerTemps( &soil);

    theResultingMeanLayerTempsShouldEqualTheMeanOriginalTemps(soil.temp, linearIncreaseTemps);
}

void test_meanLayertempsShouldRecoverIfEnthalpiesCorrespondToIncreasingTempsSoilA(){
    Soil soil;
    Real linearIncreaseTemps[NHEATGRIDP];

    setup_soil_B(&soil);
    setEnthalpiesCorrespondingToLinearIncreaseTemps(&soil, linearIncreaseTemps);
    updateMeanLayerTemps( &soil);

    theResultingMeanLayerTempsShouldEqualTheMeanOriginalTemps(soil.temp, linearIncreaseTemps);
}


void theResultingMeanLayerTempsShouldEqualTheMeanOriginalTemps(Real * layerTemps, Real  *originalTemps){
    double tempMean =0;
    for (int l = 0; l < NSOILLAYER; l++)
    {
        tempMean =0;
        for(int j=0;j<GPLHEAT; j++)
        {
            tempMean += originalTemps[GPLHEAT*l+j]/GPLHEAT;
            //printf("orTemp: %f\n", originalTemps[GPLHEAT*l+j]);
        }
        //printf("l: %d\n", l);
        TEST_ASSERT_EQUAL_DOUBLE( tempMean, layerTemps[l]);
        tempMean=0;
    }
}