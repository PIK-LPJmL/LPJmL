Real infil_perc(Stand *stand,        /**< Stand pointer */
                Real infil,          /**< rainfall + melting water - interception_stand (mm) + rw_irrig */
                Real infil_vol_enth, /**< volumetric enthalpy contained in infil (J/m^3) */
                Real dprec1,         /**< precipitation only  */
                Real *return_flow_b, /**< blue water return flow (mm) */
                int npft,            /**< number of natural PFTs */
                int ncft,            /**< number of crop PFTs */
                const Config *config /**< LPJ configuration */
               ) ;
