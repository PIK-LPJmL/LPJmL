/* headers used in the tests */
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lpj.h"

/* modules under test */
#include "update_soil_thermal_state.h"


/* mocked modules */
#include "mock_soiltemp_stubs.h"


/* global lpjml variables */
Real soildepth[] = {200.0,300.0,500.0,1000.0,1000.0,10000.0};
Config aconfig = {.percolation_heattransfer=0};


/* helpers for the tests */
#include "calc_soil_thermal_props.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "apply_enth_of_untracked_mass_shifts.h"
#include "apply_heatconduction_of_a_day.h"
#include "enth2freezefrac.h"
#include "freezefrac2soil.h"
#include "soil_therm_testing_helper.c" 
#include "apply_perc_enthalpy.h"



void applySoiltempManyTimesWithBelowSnowAndTopLayerTempInEquilibrium(Soil *);
void applyScenario1ToSoil(Soil * SoilPointer);
void changeWaterContent(Soil * defaultSoil);
void changeSoilSolidContent(Soil * defaultSoil);
void weApplySoiltemp100Times(Soil * SoilPointer, Real belowSnowforcing);
void weApplySoiltemp10Times(Soil * SoilPointer, Real belowSnowforcing);
void thenChangeTheSoilComposition(Soil * SoilPointer);
void thenChangeTheSoilCompositionDifferently(Soil * SoilPointer);
void checkIfResultsAreAsInOldModel(Soil * SoilPointer, double);
void theSoilTempShouldNotChange(Soil *, double);


void setUp (void) {
    /* Setup a soil */
    aconfig.johansen =TRUE;
} /* Is run before every test, put unit init calls here. */
void tearDown (void) {
} /* Is run after every test, put unit clean-up calls here. */





void test_theNewModelResultsShouldBeCloseToOldModelResults()
{
    /* The new model differs from the old one by the positions of the gridpoints, which are now ony within layers and not on layer boundaries */
    Soil soilA;
    Soil_thermal_prop thermProps;
    double tolerance = 0.1;
    setup_soil_A(&soilA);

    applyScenario1ToSoil(&soilA);
    
    checkIfResultsAreAsInOldModel(&soilA, tolerance);
}


void test_ifBelowSnowTempEqualsSoiltempSoiltempsShouldNotChange()
{
    Soil defaultSoil ={};
    setup_default_soil(&defaultSoil);
    Real defaultSoilTemp = defaultSoil.temp[0];

    applySoiltempManyTimesWithBelowSnowAndTopLayerTempInEquilibrium(&defaultSoil);

    theSoilTempShouldNotChange(&defaultSoil, defaultSoilTemp);
}

void test_ifWaterContentChangesWithoutWaterheatransferSoiltempsShouldNotChange()
{
    Soil defaultSoil;
    setup_default_soil(&defaultSoil);

    changeWaterContent(&defaultSoil);
    Real defaultSoilTemp = defaultSoil.temp[0];
    applySoiltempManyTimesWithBelowSnowAndTopLayerTempInEquilibrium(&defaultSoil);

    theSoilTempShouldNotChange(&defaultSoil, defaultSoilTemp);
}

void test_ifSolidContentChangesWithoutSolidheatransferSoiltempsShouldNotChange()
{
    Soil defaultSoil;
    Soil_thermal_prop th;
    setup_default_soil(&defaultSoil);

    changeSoilSolidContent(&defaultSoil);
    Real defaultSoilTemp = defaultSoil.temp[0];
    applySoiltempManyTimesWithBelowSnowAndTopLayerTempInEquilibrium(&defaultSoil);

    theSoilTempShouldNotChange(&defaultSoil, defaultSoilTemp);
}





/* Helpers */

void applyScenario1ToSoil(Soil * SoilPointer){
    weApplySoiltemp10Times(SoilPointer, -20 );
    thenChangeTheSoilComposition(SoilPointer);
    weApplySoiltemp100Times(SoilPointer, 60 );
    thenChangeTheSoilCompositionDifferently(SoilPointer);
    weApplySoiltemp10Times(SoilPointer, -30 );
    
}

void weApplySoiltemp100Times(Soil * SoilPointer, Real belowSnowforcing){
    for (int i = 0; i < 100; i++)
    {
        update_soil_thermal_state(SoilPointer, belowSnowforcing, &aconfig);
        //outputSoilTemps(SoilPointer,FALSE);
    }
}


void weApplySoiltemp10Times(Soil * SoilPointer, Real belowSnowforcing){
    for (int i = 0; i < 10; i++)
    {
        update_soil_thermal_state(SoilPointer, belowSnowforcing, &aconfig);
        //outputSoilTemps(SoilPointer,FALSE);
    }
}

void thenChangeTheSoilComposition(Soil * SoilPointer){
    SoilPointer->w[1] = min( SoilPointer->w[1] + 0.3,1);
    SoilPointer->w[2] = min( SoilPointer->w[1] + 0.3,1);
    SoilPointer->w[3] = min( SoilPointer->w[1] + 0.3,1);

    SoilPointer->wsat[4] = min(SoilPointer->wsat[4]*2,1);
    SoilPointer->wsat[5] = min(SoilPointer->wsat[5]*2,1);
    //SoilPointer->wsat[6] = min(SoilPointer->wsat[6]*2,1);

    SoilPointer->wsats[4] = SoilPointer->wsat[4]*soildepth[4];
    SoilPointer->wsats[5] = SoilPointer->wsat[5]*soildepth[5];
    //SoilPointer->wsats[6] = SoilPointer->wsat[6]*soildepth[6];
}

void thenChangeTheSoilCompositionDifferently(Soil * SoilPointer){
    SoilPointer->w_fw[1] = SoilPointer->w_fw[1]/3;
    SoilPointer->w_fw[2] = SoilPointer->w_fw[2]/3;
    SoilPointer->w_fw[3] = SoilPointer->w_fw[3]/3;
    
    SoilPointer->ice_depth[4] = SoilPointer->ice_depth[4]/2;
    SoilPointer->ice_depth[5] = SoilPointer->ice_depth[5]/2;
    SoilPointer->ice_depth[6] = SoilPointer->ice_depth[6]/2;
}

void test_noNAsShouldBeGeneratedInTempIfSoilHasNoWaterAndPores(void){
    Stand * st = setupSoilBStand();
    Real return_flow_b =0;
    Real startSoilTemp= 5.0;
    Real freezeFrac[NSOILLAYER];
    Soil_thermal_prop th;
    int l;

    foreachsoillayer(l){
        st->soil.wsats[l] = 0;
        st->soil.wsat[l] = 0;
        /*set all water variables to zero*/
        st->soil.w[l] = 0;
        st->soil.w_fw[l] = 0;
        st->soil.whcs[l] = 0;
        st->soil.whc[l] = 0;
        st->soil.wpwp[l] = 0;
        st->soil.wpwps[l] = 0;
        st->soil.ice_pwp[l] = 0;
        st->soil.ice_fw[l] = 0;
        st->soil.ice_depth[l] = 0;
    }

    setEnthalpiesCorrespondingToConstTemp(&st->soil,startSoilTemp);
    for (int i = 0; i < 10; i++)
    {
        update_soil_thermal_state(&st->soil,startSoilTemp, &aconfig);
    }

    noNAsShouldHaveBeenGeneratedInSoil(st->soil);
    teardownStand(st);
}



/*
Results of GPLHEAT=80 run of old model
temp at layer 0: -26.618446 
temp at layer 1: -11.538663 
temp at layer 2: 1.954871 
temp at layer 3: 1.565894 
temp at layer 4: -0.012880 
temp at layer 5: -0.075688 
ice cont. at layer 0: 13.000000 
ice cont. at layer 1: 27.000000 
ice cont. at layer 2: 4.357533 
ice cont. at layer 3: 287.554002 
ice cont. at layer 4: 500.150000 
ice cont. at layer 5: 575.000000 
liq. water cont. at layer 0: 0.000000 
liq. water cont. at layer 1: 0.000000 
liq. water cont. at layer 2: 443.975800 
liq. water cont. at layer 3: 892.745998 
liq. water cont. at layer 4: 0.000000 
liq. water cont. at layer 5: 0.000000 
*/


void checkIfResultsAreAsInOldModel(Soil * SoilPointer, double tolerance){
    Real oldtemps[]     = {-26.618446, -11.538663 , 1.954871  , 1.565894  , -0.012880  , -0.075688  };
    Real iceContents[]  = {13.000000 , 27.000000 , 4.357533  , 287.554002  , 500.150000  , 575.000000  };
    Real liqWatContents[]  = {0.0 , 0.0 , 443.975800  , 892.745998  , 0.0  , 0.0  };

    Real actualIceContent[NSOILLAYER];
    Real actualLiquidWaterContent[NSOILLAYER];
    int l;
    foreachsoillayer(l){
        actualIceContent[l]= allice(SoilPointer,l);
    }
    foreachsoillayer(l){
        actualLiquidWaterContent[l]= allwater(SoilPointer,l);
    }

    updateMeanLayerTemps(SoilPointer);
    foreachsoillayer(l){
        TEST_ASSERT_DOUBLE_WITHIN(tolerance, oldtemps[l], SoilPointer->temp[l]);
        TEST_ASSERT_DOUBLE_WITHIN(tolerance*80, iceContents[l], actualIceContent[l]);
        TEST_ASSERT_DOUBLE_WITHIN(tolerance*80, liqWatContents[l], actualLiquidWaterContent[l]);
    }
}

void changeWaterContent(Soil * defaultSoil){
    defaultSoil -> w[0] += 0.1;
    defaultSoil -> w[1] -= 0.1;
    defaultSoil -> ice_depth[1] -= 0.1;
    defaultSoil -> ice_fw[2] -= 0.1;
    defaultSoil -> w_fw[3] += 0.1;
    defaultSoil -> wpwp[4] += 0.1;
}

void changeSoilSolidContent(Soil * defaultSoil){
    defaultSoil -> wsat[1] -= 0.1;
    defaultSoil -> wsats[1] = soildepth[1]*defaultSoil-> wsat[1];
}

void applySoiltempManyTimesWithBelowSnowAndTopLayerTempInEquilibrium(Soil * defaultSoilPointer){
    double top_layer_temp = defaultSoilPointer->temp[0];
    for (int i = 0; i < 100; i++)
    {
        update_soil_thermal_state(defaultSoilPointer,top_layer_temp,&aconfig);
    }
}
void theSoilTempShouldNotChange(Soil * defaultSoilPointer, double defaultSoilTemp){
    int l;
    foreachsoillayer(l){
        TEST_ASSERT_DOUBLE_WITHIN(epsilon, defaultSoilTemp,defaultSoilPointer->temp[l]);
    }
}

