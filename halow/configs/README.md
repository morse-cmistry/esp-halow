# Configs

This is the home of [sdkconfig.defaults](https://docs.espressif.com/projects/idf-build-apps/en/latest/explanations/config_rules.html) that describe common combinations of ESP Boards -> Wi-Fi HaLow Hats.

# Usage

If using the registry, the project's components need to be pulled before these configs can be used. This can be done before setting up the SDKCONFIG_DEFAULTS environment variable with:
```
idf.py reconfigure
```

These can be passed in whilst setting the target to add pin, BCF, and chip type confgiurations to the project. For example these may be used during the set-target phase for setting up one of the HaLow examples as follows:

```
SDKCONFIG_DEFAULTS="sdkconfig.defaults;managed_components/morsemicro__halow/sdkconfig.defaults.seeed_xiao_esp32c3-seeed_xiao_mm6108" idf.py set-target esp32c3
```
**NOTE**  
> managed_components pathing assumes that the HaLow component is being pulled from the registry. For local components the relative path to the desired sdkconfig.default needs to be used instead.  

**NOTE**  
> On Windows the aformentioned SDKCONFIG_DEFAULTS env var need to use Windows sematics, e.g.

```
$env:SDKCONFIG_DEFAULTS = "sdkconfig.defaults;managed_components/morsemicro__halow/sdkconfig.defaults.seeed_xiao_esp32c3-seeed_xiao_mm6108" ; idf.py set-target esp32c3
```

## Naming Convention

The configs follow the following naming convention to aid in identifying the suitable configuration

```
sdkconfig.defaults.[BOARD]-[HAT]
```

The `BOARD` and `HAT` components are further broken down into:

```
[VENDOR]_[SERIES]_[MODEL]_[REVISION]
```

The series and revision may be dropped should one not exist.
If no hat model is available, it should instead reflect the Morse Micro chip (eg mm6108) made available via the hat.
