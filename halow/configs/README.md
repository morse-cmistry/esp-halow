# Configs

This is the home of sdkconfig.defaults that describe common combinations of ESP Boards -> HaLow Hats.

## Naming Convention

The configs follow the following naming convention to aid in identifying the suitable configuraton

```
sdkconfig.defaults.[BOARD]-[HAT]
```

The `BOARD` and `HAT` components are further broken down into:

```
[VENDOR]_[SERIES]_[MODEL]_[REVISION]
```

The series and revision may be dropped should one not exist.
If no hat model is available, it should instead reflect the Morse Micro chip (eg mm6108) made available via the hat.
