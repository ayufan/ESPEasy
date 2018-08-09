#ifdef USES_P097
//#######################################################################################################
//#################################### Plugin 097: Analog ###############################################
//#######################################################################################################

#define PLUGIN_097
#define PLUGIN_ID_097         97
#define PLUGIN_NAME_097       "Analog input - RMS"

#define PLUGIN_VALUENAME1_097 "Avg"
#define PLUGIN_VALUENAME2_097 "Max"
#define PLUGIN_VALUENAME3_097 "RMS"

#define MIN_SAMPLES 16
#define MAX_SAMPLES 2048

boolean Plugin_097(byte function, struct EventStruct *event, String& string)
{
  boolean success = false;

  switch (function)
  {
    case PLUGIN_DEVICE_ADD:
      {
        Device[++deviceCount].Number = PLUGIN_ID_097;
        Device[deviceCount].Type = DEVICE_TYPE_ANALOG;
        Device[deviceCount].VType = SENSOR_TYPE_SINGLE;
        Device[deviceCount].Ports = 0;
        Device[deviceCount].PullUpOption = false;
        Device[deviceCount].InverseLogicOption = false;
        Device[deviceCount].FormulaOption = true;
        Device[deviceCount].ValueCount = 3;
        Device[deviceCount].SendDataOption = true;
        Device[deviceCount].TimerOption = true;
        Device[deviceCount].GlobalSyncOption = true;
        break;
      }

    case PLUGIN_GET_DEVICENAME:
      {
        string = F(PLUGIN_NAME_097);
        break;
      }

    case PLUGIN_GET_DEVICEVALUENAMES:
      {
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[0], PSTR(PLUGIN_VALUENAME1_097));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[1], PSTR(PLUGIN_VALUENAME2_097));
        strcpy_P(ExtraTaskSettings.TaskDeviceValueNames[2], PSTR(PLUGIN_VALUENAME3_097));
        break;
      }

    case PLUGIN_WEBFORM_LOAD:
      {
        #if defined(ESP32)
          addHtml(F("<TR><TD>Analog Pin:<TD>"));
          addPinSelect(false, "taskdevicepin1", Settings.TaskDevicePin1[event->TaskIndex]);
        #endif

        addFormNumericBox(F("Samples"), F("plugin_097_samples"), Settings.TaskDevicePluginConfig[event->TaskIndex][0], MIN_SAMPLES, MAX_SAMPLES);
        success = true;
        break;
      }

    case PLUGIN_WEBFORM_SAVE:
      {
        Settings.TaskDevicePluginConfig[event->TaskIndex][0] = getFormItemInt(F("plugin_097_samples"));
        success = true;
        break;
      }

    case PLUGIN_READ:
      {
        String log = F("ADC RMS : ");

        long signalAvg = 0, signalRMS = 0, signalMax = 0, signalMin = 1024;

        int samples = Settings.TaskDevicePluginConfig[event->TaskIndex][0];
        if (samples > MAX_SAMPLES) {
          samples = MAX_SAMPLES;
        } else if (samples < MIN_SAMPLES) {
          samples = MIN_SAMPLES;
        }

        for (int i = 0; i < samples; i++)
        {
          #if defined(ESP8266)
            int16_t value = analogRead(A0);
          #endif
          #if defined(ESP32)
            int16_t value = analogRead(Settings.TaskDevicePin1[event->TaskIndex]);
          #endif

          if (value < signalMin) {
            signalMin = value;
          }
          if (value > signalMax) {
            signalMax = value;
          }
          signalAvg += value;
          signalRMS += ((long)value*value);
        }

        event->sensorType = SENSOR_TYPE_TRIPLE;
        UserVar[event->BaseVarIndex] = (float)signalAvg / samples;
        UserVar[event->BaseVarIndex+1] = (float)signalMax;
        UserVar[event->BaseVarIndex+2] = sqrt((float)signalRMS / samples);

        log += String(" Avg: ") + String(UserVar[event->BaseVarIndex], 3);
        log += String(" Max: ") + String(UserVar[event->BaseVarIndex+1], 3);
        log += String(" RMS: ") + String(UserVar[event->BaseVarIndex+2], 3);

        addLog(LOG_LEVEL_INFO,log);
        success = true;
        break;
      }
  }
  return success;
}
#endif // USES_P097
