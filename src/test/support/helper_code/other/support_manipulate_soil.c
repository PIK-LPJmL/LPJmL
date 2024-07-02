#include "lpj.h"

void update_mean_layer_temps(Soil *SoilPointer)
{
  Soil_thermal_prop thermal_props;
  calc_soil_thermal_props(UNKNOWN, &thermal_props, SoilPointer, NULL, NULL, TRUE, FALSE);
  compute_mean_layer_temps_from_enth(SoilPointer->temp, SoilPointer->enth, &thermal_props);
}


void set_enthalpies_to_half_latent_heat(Soil *SoilPointer)
{
  Soil_thermal_prop thermal_props;
  calc_soil_thermal_props(UNKNOWN, &thermal_props, SoilPointer, NULL, NULL, TRUE, TRUE);
  int l;
  foreachsoillayer(l)
  {
    for (int i = 0; i < GPLHEAT; i++)
    {
      SoilPointer->enth[l * GPLHEAT + i] = 0.5 * thermal_props.latent_heat[l * GPLHEAT + i];
    }
  }
}

void set_enthalpies_to_zero(Soil *SoilPointer)
{
  int l;
  foreachsoillayer(l)
  {
    for (int i = 0; i < GPLHEAT; i++)
    {
      SoilPointer->enth[l * GPLHEAT + i] = 0;
    }
  }
}

void set_enthalpies_corresponding_to_linearly_increasing_temps(Soil *SoilPointer, Real *linIncreaseTemps)
{
  int l;
  Real gridpointTemp;
  Soil_thermal_prop thermal_props;
  calc_soil_thermal_props(UNKNOWN, &thermal_props, SoilPointer, NULL, NULL, TRUE, FALSE);
  foreachsoillayer(l)
  {
    for (int i = 0; i < GPLHEAT; i++)
    {
      gridpointTemp = (GPLHEAT * l + i) * 0.3 - 0.5;
      linIncreaseTemps[GPLHEAT * l + i] = gridpointTemp;

      SoilPointer->enth[l * GPLHEAT + i] =
          (gridpointTemp < 0 ? gridpointTemp * thermal_props.c_frozen[l * GPLHEAT + i]
                             : gridpointTemp * thermal_props.c_unfrozen[l * GPLHEAT + i] + thermal_props.latent_heat[l * GPLHEAT + i]);
    }
  }
}


void set_enthalpies_corresponding_to_const_temp(Soil *SoilPointer, Real temp)
{
  int l;
  Real gridpointTemp;
  Soil_thermal_prop thermal_props;
  calc_soil_thermal_props(UNKNOWN, &thermal_props, SoilPointer, NULL, NULL, TRUE, FALSE);
  foreachsoillayer(l)
  {
    for (int i = 0; i < GPLHEAT; i++)
    {

      SoilPointer->enth[l * GPLHEAT + i] = (temp < 0 ? temp * thermal_props.c_frozen[l * GPLHEAT + i]
                                                     : temp * thermal_props.c_unfrozen[l * GPLHEAT + i] + thermal_props.latent_heat[l * GPLHEAT + i]);

    }
  }
  update_mean_layer_temps(SoilPointer);
}

void set_enthalpies_to_latent_heat(Soil *SoilPointer)
{
  Soil_thermal_prop thermal_props;
  calc_soil_thermal_props(UNKNOWN, &thermal_props, SoilPointer, NULL, NULL, TRUE, TRUE);
  int l;
  foreachsoillayer(l)
  {
    for (int i = 0; i < GPLHEAT; i++)
    {
      SoilPointer->enth[l * GPLHEAT + i] = thermal_props.latent_heat[l * GPLHEAT + i];
    }
  }
}

void set_soilwater_to_zero(Soil *zeroWaterSoilPointer)
{
  int l;
  foreachsoillayer(l)
  {
    zeroWaterSoilPointer->w[l] = 0;
    zeroWaterSoilPointer->wpwps[l] = 0;
    zeroWaterSoilPointer->w_fw[l] = 0;
    zeroWaterSoilPointer->ice_depth[l] = 0;
    zeroWaterSoilPointer->ice_pwp[l] = 0;
    zeroWaterSoilPointer->ice_fw[l] = 0;
  }
}
