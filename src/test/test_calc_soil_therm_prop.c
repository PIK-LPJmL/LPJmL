/* headers used in the tests */
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lpj.h"

/* modules under test */
#include "calc_soil_thermal_props.h" 

/* mocked modules */

/* global lpjml variables */
Real soildepth[] = {200.0,300.0,500.0,1000.0,1000.0,10000.0};
Config aconfig = {};

/* helpers for the tests */
#include "compute_mean_layer_temps_from_enth.h"
#include "soil_therm_testing_helper.c" 

void withItsThermalProperties(Soil_thermal_prop *, Soil);
void theThermalPropValuesShouldBeAsExpected(Soil_thermal_prop);
void theThermalConductivitiesShouldBeAsManuallyCalculatedForSoilA( Soil_thermal_prop thermProps );
void theThermalConductivityValuesShouldEqualDryValues(Soil_thermal_prop, Soil);


void setUp (void) {} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */

void test_noNAsShouldBeGeneratedInThermalPropsIfSoilHasNoWaterAndPores(void){
    Soil soil;
    Soil_thermal_prop thermProps;
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
        soil.ice_pwp[l] = 0;
        soil.ice_fw[l] = 0;
        soil.ice_depth[l] = 0;
    }

    calc_soil_thermal_props(&thermProps,&soil,NULL,NULL,TRUE,TRUE);

    for (int i = 0; i < NSOILLAYER; i++)
    {
        TEST_ASSERT_FALSE(isnan(thermProps.c_frozen[i]));    
        TEST_ASSERT_FALSE(isnan(thermProps.c_unfrozen[i])); 
        TEST_ASSERT_FALSE(isnan(thermProps.lam_frozen[i])); 
        TEST_ASSERT_FALSE(isnan(thermProps.lam_unfrozen[i])); 
        TEST_ASSERT_FALSE(isnan(thermProps.latent_heat[i])); 
    }
}

void test_soilThermalPropertiesOfDefaultSoilShouldBeCorrect(void){
    Soil soil;
    Soil_thermal_prop thermProps;
    setup_default_soil(&soil);

    calc_soil_thermal_props(&thermProps,&soil,NULL,NULL,TRUE,TRUE);

    theThermalPropValuesShouldBeAsExpected(thermProps);
}


void test_soilThermalPropertiesOfSoilAShouldBeCorrect(void){
    Soil soil;
    Soil_thermal_prop thermProps;
    setup_soil_A(&soil);
    calc_soil_thermal_props(&thermProps,&soil,NULL,NULL,TRUE,TRUE);
    theThermalConductivitiesShouldBeAsManuallyCalculatedForSoilA(thermProps);
}



void test_WaterlessSoilThermalCondunctivitiesShouldEqualDryConductivity(){
    Soil soil;
    Soil_thermal_prop thermProps;
    setup_soil_A(&soil);
    removeSoilwater(&soil);
    calc_soil_thermal_props(&thermProps,&soil,NULL,NULL,TRUE,TRUE);
    theThermalConductivityValuesShouldEqualDryValues(thermProps, soil);
}


void test_WaterlessSoilThermalCondunctivitiesShouldEqualDryConductivityForSoilB(){
    Soil soil;
    Soil_thermal_prop thermProps;
    setup_soil_B(&soil);
    removeSoilwater(&soil);
    calc_soil_thermal_props(&thermProps,&soil,NULL,NULL,TRUE,TRUE);
    theThermalConductivityValuesShouldEqualDryValues(thermProps, soil);
}

void test_WaterlessSoilThermalCondunctivitiesShouldEqualDryConductivityForSoilC(){
    Soil soil;
    Soil_thermal_prop thermProps;
    setup_soil_C(&soil);
    removeSoilwater(&soil);
    calc_soil_thermal_props(&thermProps,&soil,NULL,NULL,TRUE,TRUE);
    theThermalConductivityValuesShouldEqualDryValues(thermProps, soil);
}


void theThermalPropValuesShouldBeAsExpected( Soil_thermal_prop thermProps ){
    for (int i = 0; i < NSOILLAYER; i++)
    {
        TEST_ASSERT_EQUAL_DOUBLE( 910180,thermProps.c_frozen[i]);    
        TEST_ASSERT_EQUAL_DOUBLE( 1435180,thermProps.c_unfrozen[i]); // liquid water has a higher heat capcity than ice
        TEST_ASSERT_EQUAL_DOUBLE( 1.233783,thermProps.lam_frozen[i]); 
        TEST_ASSERT_EQUAL_DOUBLE( 0.730975,thermProps.lam_unfrozen[i]); // frozen soil conducts heat better than unfrozen
        TEST_ASSERT_EQUAL_DOUBLE( 75000000,thermProps.latent_heat[i]);
    }
}
void theThermalConductivitiesShouldBeAsManuallyCalculatedForSoilA( Soil_thermal_prop thermProps ){
     // values are calculated manually using thermal resitance formula for the elements that cross layer borders
     // res_total = res_toplayer_to_border + res_bottomlayer_to_border
     // res = dx/therm_conductivity
    TEST_ASSERT_EQUAL_DOUBLE(0.651512122, thermProps.lam_frozen[GPLHEAT]);
    TEST_ASSERT_EQUAL_DOUBLE(1.09590781, thermProps.lam_frozen[GPLHEAT*2]);
    TEST_ASSERT_EQUAL_DOUBLE(0.888385088, thermProps.lam_unfrozen[GPLHEAT*2]);
}



void theThermalConductivityValuesShouldEqualDryValues(Soil_thermal_prop thermProps, Soil theSoil){
    int l;
    foreachsoillayer(l){
        TEST_ASSERT_EQUAL_DOUBLE(theSoil.k_dry[l], thermProps.lam_frozen[l]);
        TEST_ASSERT_EQUAL_DOUBLE(theSoil.k_dry[l], thermProps.lam_unfrozen[l]);
    }
}


