/* headers used in the tests */
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lpj.h"

/* modules under test */
#include "freezefrac2soil.h"

/* mocked modules */

/* global lpjml variables */
Real soildepth[] = {200.0,300.0,500.0,1000.0,1000.0,10000.0};
Config aconfig = {};

/* helpers for the tests */
#include "compute_mean_layer_temps_from_enth.h"
#include "soil_therm_testing_helper.c" 
#include "soil_freeze_frac_testing_helper.c"
#include "enth2freezefrac.h"
#include "calc_soil_thermal_props.h" 

void theResultingWaterIceRatiosShouldMatchTheAppliedFreezeFractions(Soil *, Real *);
void recordWaterIceContent(Soil * SoilPointer, Real * waterice);
void theResultingWaterIceContentShouldBeAsBefore(Soil * SoilPointer, Real * waterIceContentBefore);
void theLatentHeatDerivedByWaterContShouldMatchSoilEnthalpy(Soil * SoilPointer);

/* At higher resolution numerical errors are more likely to be problematic */
void setUp (void) {
    /* Setup a soil */
} /* Is run before every test, put unit init calls here. */
void tearDown (void) {
} /* Is run after every test, put unit clean-up calls here. */



void test_iceToWaterIceRatioShouldMatchFreezefracAtHalfFreezing(){
    Soil soil;
    Real freezefrac[NSOILLAYER];
    setup_default_soil(&soil);

    setEnthalpiesToHalfLatentHeat(&soil);
    calculateFreezeFractions(freezefrac,&soil);
    freezefrac2soil(&soil, freezefrac);

    theResultingWaterIceRatiosShouldMatchTheAppliedFreezeFractions(&soil, freezefrac);
}

void test_iceToWaterIceRatioShouldMatchFreezefracAtZeroFreezing(){
    Soil soil;
    Real freezefrac[NSOILLAYER];
    setup_default_soil(&soil);

    setEnthalpiesToLatentHeat(&soil);
    calculateFreezeFractions(freezefrac,&soil);
    freezefrac2soil(&soil, freezefrac);

    theResultingWaterIceRatiosShouldMatchTheAppliedFreezeFractions(&soil, freezefrac);
}

void test_iceToWaterIceRatioShouldMatchFreezefracAtFullFreezing(){
    Soil soil;
    Real freezefrac[NSOILLAYER];
    setup_default_soil(&soil);

    setEnthalpiesToZero(&soil);
    calculateFreezeFractions(freezefrac,&soil);
    freezefrac2soil(&soil, freezefrac);

    theResultingWaterIceRatiosShouldMatchTheAppliedFreezeFractions(&soil, freezefrac);
}

void test_watericeTotalsShouldRemainInvariantAtHalfFreezing(){
    Soil soil;
    Real waterice[NSOILLAYER];
    Real freezefrac[NSOILLAYER];
    setup_default_soil(&soil);

    setEnthalpiesToHalfLatentHeat(&soil);
    recordWaterIceContent(&soil,waterice);
    calculateFreezeFractions(freezefrac,&soil);
    freezefrac2soil(&soil, freezefrac);

    theResultingWaterIceContentShouldBeAsBefore(&soil, waterice);
}

void test_watericeTotalsShouldRemainInvariantAtZeroFreezing(){
    Soil soil;
    Real waterice[NSOILLAYER];
    Real freezefrac[NSOILLAYER];
    setup_default_soil(&soil);

    setEnthalpiesToLatentHeat(&soil);
    recordWaterIceContent(&soil,waterice);
    calculateFreezeFractions(freezefrac,&soil);
    freezefrac2soil(&soil, freezefrac);

    theResultingWaterIceContentShouldBeAsBefore(&soil, waterice);
}

void test_watericeTotalsShouldRemainInvariantAtFullFreezing(){
    Soil soil;
    Real waterice[NSOILLAYER];
    Real freezefrac[NSOILLAYER];
    setup_default_soil(&soil);

    setEnthalpiesToZero(&soil);
    recordWaterIceContent(&soil,waterice);
    calculateFreezeFractions(freezefrac,&soil);
    freezefrac2soil(&soil, freezefrac);

    theResultingWaterIceContentShouldBeAsBefore(&soil, waterice);
}

void test_capillaryWaterTotalShouldRemainInvariantAtHalfFreezing(){
    Soil soil;
    Real capillaryWaterBefore[NSOILLAYER];
    Real capillaryWaterAfter[NSOILLAYER];
    Real freezefrac[NSOILLAYER];
    setup_default_soil(&soil);

    setEnthalpiesToHalfLatentHeat(&soil);
    recordCapillaryWaterContent(&soil,capillaryWaterBefore);
    calculateFreezeFractions(freezefrac,&soil);
    freezefrac2soil(&soil, freezefrac);
    recordCapillaryWaterContent(&soil,capillaryWaterAfter);

    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(capillaryWaterBefore,capillaryWaterAfter,NSOILLAYER);

}

void test_freeWaterTotalShouldRemainInvariantAtHalfFreezing(){
    Soil soil;
    Real freeWaterBefore[NSOILLAYER];
    Real freeWaterAfter[NSOILLAYER];
    Real freezefrac[NSOILLAYER];
    setup_default_soil(&soil);

    setEnthalpiesToHalfLatentHeat(&soil);
    recordFreeWaterContent(&soil,freeWaterBefore);
    calculateFreezeFractions(freezefrac,&soil);
    freezefrac2soil(&soil, freezefrac);
    recordFreeWaterContent(&soil,freeWaterAfter);

    TEST_ASSERT_EQUAL_DOUBLE_ARRAY(freeWaterBefore,freeWaterAfter,NSOILLAYER);

}



void test_waterContentDerivedEnthalpyShouldMatchActualEnthalpyAtHalfFreezing(){
    Soil soil;
    Real waterice[NSOILLAYER];
    Real freezefrac[NSOILLAYER];
    setup_soil_A(&soil);

    setEnthalpiesToHalfLatentHeat(&soil);
    calculateFreezeFractions(freezefrac,&soil);
    freezefrac2soil(&soil, freezefrac);

    theLatentHeatDerivedByWaterContShouldMatchSoilEnthalpy(&soil);
}



void test_noNAsShouldBeGeneratedWhenFreezefracsAreAppliedToWaterlessSoil(){
    Soil soil;
    Real waterice[NSOILLAYER];
    Real freezefrac[NSOILLAYER];

    setup_default_soil(&soil);
    removeSoilwater(&soil);
    calculateFreezeFractions(freezefrac,&soil);
    freezefrac2soil(&soil, freezefrac);
    noNAsShouldHaveBeenGeneratedInSoil(soil);
}

void test_noNAsShouldBeGeneratedForWaterlessAndPorelessSoil(){
    Soil soil;
    Real waterice[NSOILLAYER];
    Real freezefrac[NSOILLAYER];

    setup_default_soil(&soil);
    removeSoilwater(&soil);
    int l;
    foreachsoillayer(l){
        soil.wsats[l] = 0;
        soil.wsat[l] = 0;
        soil.whcs[l] = 0;
        soil.whc[l] = 0;
    }

    calculateFreezeFractions(freezefrac,&soil);
    freezefrac2soil(&soil, freezefrac);
    noNAsShouldHaveBeenGeneratedInSoil(soil);
}




void recordWaterIceContent(Soil * SoilPointer, Real * waterice){
    int l;
    foreachsoillayer(l){
        waterice[l]=allwater(SoilPointer, l)+ allice(SoilPointer, l);
    }
}

void recordCapillaryWaterContent(Soil * SoilPointer, Real * waterice){
    int l;
    foreachsoillayer(l){
        waterice[l]=SoilPointer->w[l]* SoilPointer->whcs[l]+SoilPointer->ice_depth[l];
    }
}

void recordFreeWaterContent(Soil * SoilPointer, Real * waterice){
    int l;
    foreachsoillayer(l){
        waterice[l]=SoilPointer->w_fw[l]+SoilPointer->ice_fw[l];
    }
}



void theResultingWaterIceRatiosShouldMatchTheAppliedFreezeFractions(Soil * SoilPointer, Real * freezefrac){
    int l;
    double ice_ratio;
    foreachsoillayer(l){
        ice_ratio = allice(SoilPointer,l)/(allwater(SoilPointer,l)+allice(SoilPointer,l));
        TEST_ASSERT_EQUAL_DOUBLE( freezefrac[l], ice_ratio);
    }
}


void theResultingWaterIceContentShouldBeAsBefore(Soil * SoilPointer, Real * waterIceContentBefore){
    int l;
    double waterIceContent;
    foreachsoillayer(l){
        waterIceContent = (allwater(SoilPointer,l)+allice(SoilPointer,l));
        TEST_ASSERT_EQUAL_DOUBLE(waterIceContentBefore[l], waterIceContent);
    }
}

void theLatentHeatDerivedByWaterContShouldMatchSoilEnthalpy(Soil * SoilPointer){
    int l;
    Real latentHeatFromWaterCont;
    foreachsoillayer(l){
        latentHeatFromWaterCont = allwater(SoilPointer,l)/1000*c_water2ice/(soildepth[l]/1000);
        TEST_ASSERT_EQUAL_DOUBLE(latentHeatFromWaterCont, SoilPointer->enth[GPLHEAT*l]);
    }
}
