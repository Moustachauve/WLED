#include "wled_alexa.h"
#include "wled.h"
#include "src/dependencies/espalexa/EspalexaDevice.h"
#include "const.h"

#ifndef WLED_DISABLE_ALEXA
void onAlexaChange(EspalexaDevice* dev);

void alexaInit()
{
  if (alexaEnabled && WLED_CONNECTED)
  {
    if (espalexaDevice == nullptr) //only init once
    {
      espalexaDevice = new EspalexaDevice(alexaInvocationName, onAlexaChange, EspalexaDeviceType::extendedcolor);
      espalexa.addDevice(espalexaDevice);
      espalexa.begin(&server);
    } else {
      espalexaDevice->setName(alexaInvocationName);
    }
  }
}

void handleAlexa()
{
  if (!alexaEnabled || !WLED_CONNECTED) return;
  espalexa.loop();
}

void onAlexaChange(EspalexaDevice* dev)
{
  EspalexaDeviceProperty m = espalexaDevice->getLastChangedProperty();
  
  if (m == EspalexaDeviceProperty::on)
  {
    if (!macroAlexaOn)
    {
      if (bri == 0)
      {
        bri = briLast;
        colorUpdated(NOTIFIER_CALL_MODE_ALEXA);
      }
    } else applyMacro(macroAlexaOn);
  } else if (m == EspalexaDeviceProperty::off)
  {
    if (!macroAlexaOff)
    {
      if (bri > 0)
      {
        briLast = bri;
        bri = 0;
        colorUpdated(NOTIFIER_CALL_MODE_ALEXA);
      }
    } else applyMacro(macroAlexaOff);
  } else if (m == EspalexaDeviceProperty::bri)
  {
    bri = espalexaDevice->getValue();
    colorUpdated(NOTIFIER_CALL_MODE_ALEXA);
  } else //color
  {
    if (espalexaDevice->getColorMode() == EspalexaColorMode::ct) //shade of white
    {
      uint16_t ct = espalexaDevice->getCt();
      if (useRGBW)
      {
        switch (ct) { //these values empirically look good on RGBW
          case 199: col[0]=255; col[1]=255; col[2]=255; col[3]=255; break;
          case 234: col[0]=127; col[1]=127; col[2]=127; col[3]=255; break;
          case 284: col[0]=  0; col[1]=  0; col[2]=  0; col[3]=255; break;
          case 350: col[0]=130; col[1]= 90; col[2]=  0; col[3]=255; break;
          case 383: col[0]=255; col[1]=153; col[2]=  0; col[3]=255; break;
        }
      } else {
        colorCTtoRGB(ct, col);
      }
    } else {
      uint32_t color = espalexaDevice->getRGB();
    
      col[0] = ((color >> 16) & 0xFF);
      col[1] = ((color >>  8) & 0xFF);
      col[2] = ( color        & 0xFF);
    }
    colorUpdated(NOTIFIER_CALL_MODE_ALEXA);
  }
}


#else
 void alexaInit(){}
 void handleAlexa(){}
#endif
