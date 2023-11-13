/* headers used in the tests */
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lpj.h"

/* modules under test */
#include "infil_perc_rain.h"
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

/* change made to the watercontent of soillayer without accounting for the respective enthalpy change, could be from some function where the enthalpy of e.g. water inflow is no known or where water heat transfer is not (yet) considered */
#define UNACCOUNTED_WATER_CHANGE 0.1; 

void setUp (void) {
        aconfig.johansen=TRUE;
} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */
void makeEnthalpyTrackedWaterChange(Soil * soil, Real water_inflow, Real enth_of_water, Config aconfig);
void makeEnthalpyUntrackedWaterChange(Soil * soil, Real water_inflow);


void test_soiltempShouldRemainConstantIfInflowingWaterHasSameTemp(void){
    Stand * st = setupSoilBStand();
    Real temp= 10.0;
    Real water_inflow = 15.0;    
    Real enth_of_water;
    setEnthalpiesCorrespondingToConstTemp(&st->soil,temp);

    Soil *  so = &(st->soil);

    /* The order of unaccounted and accounted water changes should not matter*/
    /* The only important thing is that enthalpy of accounted water changes is applied before treating unaccounted changes */

    makeEnthalpyUntrackedWaterChange(&st->soil,10);
    apply_perc_enthalpy(&st->soil);

    makeEnthalpyTrackedWaterChange(&st->soil,-10,temp*c_water+c_water2ice,aconfig);
    makeEnthalpyUntrackedWaterChange(&st->soil,10);
    makeEnthalpyTrackedWaterChange(&st->soil,5,temp*c_water+c_water2ice,aconfig);
    apply_perc_enthalpy(&st->soil);

    makeEnthalpyUntrackedWaterChange(&st->soil,10);

    modify_enth_due_to_unaccounted_masschanges(&st->soil,&aconfig);
    updateMeanLayerTemps(&st->soil);


    TEST_ASSERT_EACH_EQUAL_DOUBLE(temp,st->soil.temp,NSOILLAYER);
    teardownStand(st);
    
}

void makeEnthalpyTrackedWaterChange(Soil * soil, Real water_inflow, Real enth_of_water, Config aconfig){
    int l;
    foreachsoillayer(l){
        reconcile_layer_energy_with_water_shift(soil,l,water_inflow,enth_of_water, (&aconfig) );
        soil->w[l]+=water_inflow/soil->whcs[l];
    }
}

void makeEnthalpyUntrackedWaterChange(Soil * soil, Real water_inflow){
    int l;
    foreachsoillayer(l){
        soil->w[l]+=water_inflow/soil->whcs[l];
    }
}