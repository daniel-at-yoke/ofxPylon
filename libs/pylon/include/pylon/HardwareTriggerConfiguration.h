//-----------------------------------------------------------------------------
//  Basler pylon SDK
//  Copyright (c) 2010-2019 Basler AG
//  http://www.baslerweb.com
//  Author:  Andreas Gau
//
//  Adapted by Daniel Ohrgaard 
//-----------------------------------------------------------------------------
/*!
\file
\brief An instant camera configuration for a hardware trigger,
       Adapted from SoftwareTriggerConfiguration.h
*/

#ifndef INCLUDED_HARDWARETRIGGERCONFIGURATION_H_4655834
#define INCLUDED_HARDWARETRIGGERCONFIGURATION_H_4655834

#include <pylon/Platform.h>

#ifdef _MSC_VER
#   pragma pack(push, PYLON_PACKING)
#endif /* _MSC_VER */

#include <pylon/InstantCamera.h>
#include <pylon/ParameterIncludes.h>

namespace Pylon
{
    /** \addtogroup Pylon_InstantCameraApiGeneric
     * @{
     */

    /*!
    \class  CHardwareTriggerConfiguration
    \brief  Changes the configuration of the camera so that the acquisition of frames is triggered by a hardware trigger.
    */
    class CHardwareTriggerConfiguration : public CConfigurationEventHandler
    {
    public:
        /// Apply hardware trigger configuration.
        static void ApplyConfiguration( GENAPI_NAMESPACE::INodeMap& nodemap)
        {
            using namespace GENAPI_NAMESPACE;

            // Disable all trigger types except the trigger type used for triggering the acquisition of
            // frames.
            {
                // Get required enumerations.
                CEnumParameter triggerSelector(nodemap, "TriggerSelector");
                CEnumParameter triggerMode(nodemap, "TriggerMode");

                // Check the available camera trigger mode(s) to select the appropriate one: acquisition start trigger mode
                // (used by older cameras, i.e. for cameras supporting only the legacy image acquisition control mode;
                // do not confuse with acquisition start command) or frame start trigger mode
                // (used by newer cameras, i.e. for cameras using the standard image acquisition control mode;
                // equivalent to the acquisition start trigger mode in the legacy image acquisition control mode).
                String_t triggerName("FrameStart");
                if (!triggerSelector.CanSetValue(triggerName))
                {
                    triggerName = "AcquisitionStart";
                    if (!triggerSelector.CanSetValue(triggerName))
                    {
                        throw RUNTIME_EXCEPTION("Could not select trigger. Neither FrameStart nor AcquisitionStart is available.");
                    }
                }

                // Get all enumeration entries of trigger selector.
                StringList_t triggerSelectorEntries;
                triggerSelector.GetSettableValues(triggerSelectorEntries);

                // Turn trigger mode off for all trigger selector entries except for the frame trigger given by triggerName.
                for (StringList_t::const_iterator it = triggerSelectorEntries.begin(); it != triggerSelectorEntries.end(); ++it)
                {
                    // Set trigger mode to off.
                    triggerSelector.SetValue(*it);
                    if (triggerName == *it)
                    {
                        // Activate trigger.
                        triggerMode.SetValue("On");

                        // The trigger source must be set to the trigger input, e.g. 'Line1'.
                        CEnumParameter(nodemap, "TriggerSource").SetValue("Line1");

                        //The trigger activation must be set to e.g. 'RisingEdge'.
                        CEnumParameter(nodemap, "TriggerActivation").SetValue("RisingEdge");
                    }
                    else
                    {
                        triggerMode.SetValue("Off");
                    }
                }
                // Finally select the frame trigger type (resp. acquisition start type
                // for older cameras). Issuing a software trigger will now trigger
                // the acquisition of a frame.
                triggerSelector.SetValue(triggerName);
            }


            //Set acquisition mode to "continuous"
            CEnumParameter(nodemap, "AcquisitionMode").SetValue("Continuous");
        }

        //Set basic camera settings.
        virtual void OnOpened( CInstantCamera& camera)
        {
            try
            {
                ApplyConfiguration( camera.GetNodeMap());
            }
            catch (const GenericException& e)
            {
                throw RUNTIME_EXCEPTION( "Could not apply configuration. Pylon::GenericException caught in OnOpened method msg=%hs", e.what());
            }
            catch (const std::exception& e)
            {
                throw RUNTIME_EXCEPTION( "Could not apply configuration. std::exception caught in OnOpened method msg=%hs", e.what());
            }
            catch (...)
            {
                throw RUNTIME_EXCEPTION( "Could not apply configuration. Unknown exception caught in OnOpened method.");
            }
        }
    };

    /**
     * @}
     */
}

#ifdef _MSC_VER
#   pragma pack(pop)
#endif /* _MSC_VER */

#endif /* INCLUDED_SOFTWARETRIGGERCONFIGURATION_H_4655834 */
