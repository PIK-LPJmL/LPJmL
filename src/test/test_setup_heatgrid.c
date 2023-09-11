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
Config aconfig = {};

/* helpers for the tests */
#include "calc_soil_thermal_props.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "apply_enth_of_untracked_mass_shifts.h"
#include "apply_heatconduction_of_a_day.h"
#include "enth2freezefrac.h"
#include "freezefrac2soil.h"
#include "soil_therm_testing_helper.c" 
#include "apply_perc_enthalpy.h"


void test_locationsOfGridpointsInLayersShouldBeAccordingToDesign()
{
    Real h [NSOILLAYER*GPLHEAT];
    setup_heatgrid(h);
    TEST_ASSERT_EQUAL_DOUBLE(0.2/(GPLHEAT*2), h[0]);
    if(GPLHEAT>1){
      TEST_ASSERT_EQUAL_DOUBLE(0.2/(GPLHEAT), h[1]);
    }
    TEST_ASSERT(h[GPLHEAT]<h[GPLHEAT+1]);
    /* Code to print the poisitons of all gridpoints
    Real gridpoint_position=0;
    for (int i = 0; i < NSOILLAYER*GPLHEAT; i++)
    {
        printf("Position of gp %d: %f \n" , i,gridpoint_position);
        gridpoint_position+=h[i];
    }
    */
}