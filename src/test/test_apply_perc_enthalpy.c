/* headers used in the tests */
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lpj.h"

/* modules under test */
#include "apply_perc_enthalpy.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "calc_soil_thermal_props.h" 

/* mocked modules */

/* global lpjml variables */
Real soildepth[] = {200.0,300.0,500.0,1000.0,1000.0,10000.0};
Config aconfig = {};

/* helpers for the tests */
#include "soil_therm_testing_helper.c" 

void setUp (void) {
        aconfig.johansen=TRUE;
} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */


void test_soiltempShouldRemainConstantIfInflowingWaterHasSameTemp(void){
    Stand * st = setupDefaultStand();
    Real temp= 10.0;
    Real water_inflow = 15.0;

    setEnthalpiesCorrespondingToConstTemp(&st->soil,temp);
    Real enth_of_water = temp*c_water+c_water2ice;
    st->soil.perc_energy[0]=enth_of_water*water_inflow/1000;
    st->soil.wi_abs_enth_adj[0]+=water_inflow;
    st->soil.w[0]+=water_inflow/st->soil.whcs[0];
    apply_perc_enthalpy(&st->soil);
    updateMeanLayerTemps(&st->soil);
    TEST_ASSERT_EQUAL_DOUBLE(temp,st->soil.temp[0]);
    teardownStand(st);
    
}

void test_mixingtempOfSoilAndWaterInflowShouldBeCorrect(void){    
    Real watertemp= 10.0;
    Real water_inflow = 15.0;
    Real soiltemp = 5.0;
    Stand * standP= setupDefaultStand();

    setEnthalpiesCorrespondingToConstTemp(&standP->soil,soiltemp);
    Real enth_of_water = watertemp*c_water+c_water2ice;
    Real waterc_abs_layer = allwater((&(standP->soil)),0)+allice((&(standP->soil)),0);
    Real solidc_abs_layer = soildepth[0] - (&standP->soil)->wsats[0];
    Real c_soil = (c_mineral * solidc_abs_layer + c_water * waterc_abs_layer) / soildepth[0];
    Real expected_mixing_temp= (soiltemp*c_soil*soildepth[0]+watertemp*c_water*water_inflow)/(c_soil*soildepth[0]+c_water*water_inflow);
    standP->soil.perc_energy[0]=enth_of_water*water_inflow/1000;
    standP->soil.wi_abs_enth_adj[0]+=water_inflow;
    standP->soil.w[0]+=water_inflow/standP->soil.whcs[0];
    apply_perc_enthalpy(&standP->soil);
    updateMeanLayerTemps(&standP->soil);

    TEST_ASSERT_EQUAL_DOUBLE(expected_mixing_temp,standP->soil.temp[0]);
    teardownStand(standP);
}
