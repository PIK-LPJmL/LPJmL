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
Config aconfig = {.percolation_heattransfer=1};


/* helpers for the tests */
#include "calc_soil_thermal_props.h"
#include "compute_mean_layer_temps_from_enth.h"
#include "apply_enth_of_untracked_mass_shifts.h"
#include "apply_heatconduction_of_a_day.h"
#include "enth2freezefrac.h"
#include "freezefrac2soil.h"
#include "soil_therm_testing_helper.c" 
#include "apply_perc_enthalpy.h"


void theSoilHeatShouldBeCloseToAnalyticSolutionAtDayOne(double tolerance, Soil someSoil);
void theSoilHeatShouldBeCloseToAnalyticSolutionAtDayTwo(double tolerance, Soil someSoil);
void check20cmand50cmSoilTemps(double expected_temp_at_20cm,double expected_temp_at_50cm,double tolerance,Soil someSoil);
void checkLayer2and4Temps(double expected_temp_layer2, double expected_temp_layer4,double tolerance,Soil someSoil);

#define TOLERANCE 0.0001

void setUp (void) {
    aconfig.johansen =TRUE;

} /* Is run before every test, put unit init calls here. */
void tearDown (void) {} /* Is run after every test, put unit clean-up calls here. */



void test_afterOneHeatConductionDayTempsShouldMatchAnalyticalSolution(void){
    Soil defaulSoil;
    double tolerance =TOLERANCE;
    setup_default_soil(&defaulSoil);

    update_soil_thermal_state(&defaulSoil,2,&aconfig);

    theSoilHeatShouldBeCloseToAnalyticSolutionAtDayOne(tolerance,defaulSoil);
}

void test_afterTwoHeatConductionDaysTempsShouldMatchAnalyticalSolution(void){

    Soil defaulSoil;
    double tolerance =TOLERANCE;
    setup_default_soil(&defaulSoil);

    update_soil_thermal_state(&defaulSoil,2,&aconfig);
    update_soil_thermal_state(&defaulSoil,2,&aconfig);

    theSoilHeatShouldBeCloseToAnalyticSolutionAtDayTwo(tolerance,defaulSoil);
}




void theSoilHeatShouldBeCloseToAnalyticSolutionAtDayOne(double tolerance,Soil someSoil){
    double analyticalMean0_5to1mDay1 = -9.377364007590378;
    double analyticalMean2to3mDay1 = -10.986795388286655;
    checkLayer2and4Temps(analyticalMean0_5to1mDay1, analyticalMean2to3mDay1, tolerance, someSoil);
    
    //double analyticalSolat20cmDay1 = -3.04191943663280;
    //double analyticalSolat50cmDay1 = -7.46058373565938;
    //check20cmand50cmSoilTemps(analyticalSolat20cmDay1, analyticalSolat50cmDay1, tolerance, someSoil);
}

void theSoilHeatShouldBeCloseToAnalyticSolutionAtDayTwo(double tolerance,Soil someSoil){
    double analyticalMean0_5to1mDay1 = -7.669178578924899;
    double analyticalMean2to3mDay1 =  -10.978824688456244;
    checkLayer2and4Temps(analyticalMean0_5to1mDay1, analyticalMean2to3mDay1, tolerance, someSoil);
}

void check20cmand50cmSoilTemps(double expected_temp_at_20cm,double expected_temp_at_50cm,double tolerance,Soil someSoil){
    Soil_thermal_prop th;
    calc_soil_thermal_props(&th,&someSoil, NULL,NULL,TRUE,TRUE);
    int gridpointat20cm = GPLHEAT-1;
    TEST_ASSERT_DOUBLE_WITHIN(tolerance, expected_temp_at_20cm ,
                             ENTH2TEMP(someSoil.enth, th, gridpointat20cm));

    int gridpointat50cm = 2*GPLHEAT-1;
    TEST_ASSERT_DOUBLE_WITHIN(tolerance, expected_temp_at_50cm ,
                             ENTH2TEMP(someSoil.enth, th, gridpointat50cm));
}

void checkLayer2and4Temps(double expected_temp_layer2, double expected_temp_layer4,double tolerance,Soil someSoil){
    Soil_thermal_prop th;
    //calc_soil_thermal_props(&th,&someSoil, NULL,NULL,TRUE,TRUE);
    TEST_ASSERT_DOUBLE_WITHIN(tolerance, expected_temp_layer2 , someSoil.temp[2]);
    TEST_ASSERT_DOUBLE_WITHIN(tolerance, expected_temp_layer4 , someSoil.temp[4]);
}
