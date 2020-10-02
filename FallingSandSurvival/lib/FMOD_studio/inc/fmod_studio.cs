/* ======================================================================================== */
/* FMOD Studio API - C# wrapper.                                                            */
/* Copyright (c), Firelight Technologies Pty, Ltd. 2004-2020.                               */
/*                                                                                          */
/* For more detail visit:                                                                   */
/* https://fmod.com/resources/documentation-api?version=2.0&page=page=studio-api.html       */
/* ======================================================================================== */

using System;
using System.Text;
using System.Runtime.InteropServices;
using System.Collections;

namespace FMOD.Studio
{
    public class STUDIO_VERSION
    {
#if (UNITY_IPHONE || UNITY_TVOS || UNITY_SWITCH || UNITY_WEBGL) && !UNITY_EDITOR
        public const string dll     = "__Internal";
#elif (UNITY_PS4) && DEVELOPMENT_BUILD
        public const string dll     = "libfmodstudioL";
#elif (UNITY_PS4 || UNITY_WIIU || UNITY_PSP2) && !UNITY_EDITOR
        public const string dll     = "libfmodstudio";
#elif UNITY_EDITOR || ((UNITY_STANDALONE || UNITY_ANDROID || UNITY_XBOXONE || UNITY_STADIA) && DEVELOPMENT_BUILD)
        public const string dll     = "fmodstudioL";
#else
        public const string dll     = "fmodstudio";
#endif
    }

    public enum STOP_MODE : int
    {
        ALLOWFADEOUT,              /* Allows AHDSR modulators to complete their release, and DSP effect tails to play out. */
        IMMEDIATE,                 /* Stops the event instance immediately. */
    }

    public enum LOADING_STATE : int
    {
        UNLOADING,        /* Currently unloading. */
        UNLOADED,         /* Not loaded. */
        LOADING,          /* Loading in progress. */
        LOADED,           /* Loaded and ready to play. */
        ERROR,            /* Failed to load and is now in error state. */
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PROGRAMMER_SOUND_PROPERTIES
    {
        public StringWrapper name;
        public IntPtr sound;
        public int subsoundIndex;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct TIMELINE_MARKER_PROPERTIES
    {
        public StringWrapper name;
        public int position;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct TIMELINE_BEAT_PROPERTIES
    {
        public int bar;
        public int beat;
        public int position;
        public float tempo;
        public int timesignatureupper;
        public int timesignaturelower;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct ADVANCEDSETTINGS
    {
        public int cbsize;                  /* [w]   Size of this structure.  NOTE: For C# wrapper, users can leave this at 0. ! */
        public int commandqueuesize;        /* [r/w] Optional. Specify 0 to ignore. Specify the command queue size for studio async processing.  Default 4096 (4kb) */
        public int handleinitialsize;       /* [r/w] Optional. Specify 0 to ignore. Specify the initial size to allocate for handles.  Memory for handles will grow as needed in pages. */
        public int studioupdateperiod;      /* [r/w] Optional. Specify 0 to ignore. Specify the update period of Studio when in async mode, in milliseconds.  Will be quantised to the nearest multiple of mixer duration.  Default is 20ms. */
        public int idlesampledatapoolsize;  /* [r/w] Optional. Specify 0 to ignore. Specify the amount of sample data to keep in memory when no longer used, to avoid repeated disk IO.  Use -1 to disable.  Default is 256kB. */
        public int streamingscheduledelay;  /* [r/w] Optional. Specify 0 to ignore. Specify the schedule delay for streams, in samples.  Lower values can reduce latency when scheduling events containing streams but may cause scheduling issues if too small. Default is 8192 samples. */
        public IntPtr encryptionkey;        /* [w]   Optional. Specify 0 to ignore. Specify the key for loading sounds from encrypted banks. */
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CPU_USAGE
    {
        public float dspusage;            /* Returns the % CPU time taken by DSP processing on the low level mixer thread. */
        public float streamusage;         /* Returns the % CPU time taken by stream processing on the low level stream thread. */
        public float geometryusage;       /* Returns the % CPU time taken by geometry processing on the low level geometry thread. */
        public float updateusage;         /* Returns the % CPU time taken by low level update, called as part of the studio update. */
        public float studiousage;         /* Returns the % CPU time taken by studio update, called from the studio thread. Does not include low level update time. */
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct BUFFER_INFO
    {
        public int currentusage;                    /* Current buffer usage in bytes. */
        public int peakusage;                       /* Peak buffer usage in bytes. */
        public int capacity;                        /* Buffer capacity in bytes. */
        public int stallcount;                      /* Number of stalls due to buffer overflow. */
        public float stalltime;                     /* Amount of time stalled due to buffer overflow, in seconds. */
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct BUFFER_USAGE
    {
        public BUFFER_INFO studiocommandqueue;      /* Information for the Studio Async Command buffer, controlled by FMOD_STUDIO_ADVANCEDSETTINGS commandqueuesize. */
        public BUFFER_INFO studiohandle;            /* Information for the Studio handle table, controlled by FMOD_STUDIO_ADVANCEDSETTINGS handleinitialsize. */
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct BANK_INFO
    {
        public int size;                            /* The size of this struct (for binary compatibility) */
        public IntPtr userdata;                     /* User data to be passed to the file callbacks */
        public int userdatalength;                  /* If this is non-zero, userdata will be copied internally */
        public FILE_OPEN_CALLBACK opencallback;     /* Callback for opening this file. */
        public FILE_CLOSE_CALLBACK closecallback;   /* Callback for closing this file. */
        public FILE_READ_CALLBACK readcallback;     /* Callback for reading from this file. */
        public FILE_SEEK_CALLBACK seekcallback;     /* Callback for seeking within this file. */
    }

    [Flags]
    public enum SYSTEM_CALLBACK_TYPE : uint
    {
        PREUPDATE = 0x00000001,             /* Called at the start of the main Studio update.  For async mode this will be on its own thread. */
        POSTUPDATE = 0x00000002,            /* Called at the end of the main Studio update.  For async mode this will be on its own thread. */
        BANK_UNLOAD = 0x00000004,           /* Called when bank has just been unloaded, after all resources are freed. CommandData will be the bank handle.*/
        ALL = 0xFFFFFFFF,                   /* Pass this mask to Studio::System::setCallback to receive all callback types. */
    }

    public delegate RESULT SYSTEM_CALLBACK(IntPtr system, SYSTEM_CALLBACK_TYPE type, IntPtr commanddata, IntPtr userdata);

    public enum PARAMETER_TYPE : int
    {
        GAME_CONTROLLED,                    /* Controlled via the API using Studio::ParameterInstance::setValue. */
        AUTOMATIC_DISTANCE,                 /* Distance between the event and the listener. */
        AUTOMATIC_EVENT_CONE_ANGLE,         /* Angle between the event's forward vector and the vector pointing from the event to the listener (0 to 180 degrees). */
        AUTOMATIC_EVENT_ORIENTATION,        /* Horizontal angle between the event's forward vector and listener's forward vector (-180 to 180 degrees). */
        AUTOMATIC_DIRECTION,                /* Horizontal angle between the listener's forward vector and the vector pointing from the listener to the event (-180 to 180 degrees). */
        AUTOMATIC_ELEVATION,                /* Angle between the listener's XZ plane and the vector pointing from the listener to the event (-90 to 90 degrees). */
        AUTOMATIC_LISTENER_ORIENTATION,     /* Horizontal angle between the listener's forward vector and the global positive Z axis (-180 to 180 degrees). */
        AUTOMATIC_SPEED,                    /* Magnitude of the relative velocity of the event and the listener */
        MAX
    }

    [Flags]
    public enum PARAMETER_FLAGS : uint
    {
        READONLY      = 0x00000001,     /* The parameter is read-only. Its value cannot be set from the API. */
        AUTOMATIC     = 0x00000002,     /* The parameter is automatic. See FMOD_STUDIO_PARAMETER_TYPE. */
        GLOBAL        = 0x00000004,     /* The parameter is global. All instances share the same value. */
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PARAMETER_ID
    {
        public uint data1;  /* The first half of the ID. */
        public uint data2;  /* The second half of the ID. */
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PARAMETER_DESCRIPTION
    {
        public StringWrapper name;          /* Name of the parameter. */
        public PARAMETER_ID id;             /* ID of the parameter. */
        public float minimum;               /* Minimum parameter value. */
        public float maximum;               /* Maximum parameter value. */
        public float defaultvalue;          /* Default parameter value. */
        public PARAMETER_TYPE type;         /* Type of the parameter. */
        public PARAMETER_FLAGS flags;       /* Flags describing the behavior of the parameter. */
    }

    // This is only need for loading memory and given our C# wrapper LOAD_MEMORY_POINT isn't feasible anyway
    enum LOAD_MEMORY_MODE : int
    {
        LOAD_MEMORY,
        LOAD_MEMORY_POINT,
    }

    enum LOAD_MEMORY_ALIGNMENT : int
    {
        VALUE = 32
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct SOUND_INFO
    {
        public IntPtr name_or_data;         /* The filename or memory buffer that contains the sound. */
        public MODE mode;                   /* Mode flags required for loading the sound. */
        public CREATESOUNDEXINFO exinfo;    /* Extra information required for loading the sound. */
        public int subsoundindex;           /* Subsound index for loading the sound. */

        public string name
        {
            get
            {
                using (StringHelper.ThreadSafeEncoding encoding = StringHelper.GetFreeHelper())
                {
                    return ((mode & (MODE.OPENMEMORY | MODE.OPENMEMORY_POINT)) == 0) ? encoding.stringFromNative(name_or_data) : String.Empty;
                }
            }
        }
    }

    public enum USER_PROPERTY_TYPE : int
    {
        INTEGER,         /* Integer property */
        BOOLEAN,         /* Boolean property */
        FLOAT,           /* Float property */
        STRING,          /* String property */
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct USER_PROPERTY
    {
        public StringWrapper name;                     /* Name of the user property. */
        public USER_PROPERTY_TYPE type;                /* Type of the user property. Use this to select one of the following values. */
        private Union_IntBoolFloatString value;

        public int intValue()       {   return (type == USER_PROPERTY_TYPE.INTEGER) ? value.intvalue : -1;      }
        public bool boolValue()     {   return (type == USER_PROPERTY_TYPE.BOOLEAN) ? value.boolvalue : false;  }
        public float floatValue()   {   return (type == USER_PROPERTY_TYPE.FLOAT)   ? value.floatvalue : -1;    }
        public string stringValue() {   return (type == USER_PROPERTY_TYPE.STRING)  ? value.stringvalue : "";   }
    };

    [StructLayout(LayoutKind.Explicit)]
    struct Union_IntBoolFloatString
    {
        [FieldOffset(0)]
        public int intvalue;
        [FieldOffset(0)]
        public bool boolvalue;
        [FieldOffset(0)]
        public float floatvalue;
        [FieldOffset(0)]
        public StringWrapper stringvalue;
    }

    [Flags]
    public enum INITFLAGS : uint
    {
        NORMAL                  = 0x00000000,   /* Initialize normally. */
        LIVEUPDATE              = 0x00000001,   /* Enable live update. */
        ALLOW_MISSING_PLUGINS   = 0x00000002,   /* Load banks even if they reference plugins that have not been loaded. */
        SYNCHRONOUS_UPDATE      = 0x00000004,   /* Disable asynchronous processing and perform all processing on the calling thread instead. */
        DEFERRED_CALLBACKS      = 0x00000008,   /* Defer timeline callbacks until the main update. See Studio::EventInstance::setCallback for more information. */
        LOAD_FROM_UPDATE        = 0x00000010,   /* No additional threads are created for bank and resource loading.  Loading is driven from Studio::System::update.  Mainly used in non-realtime situations. */
    }

    [Flags]
    public enum LOAD_BANK_FLAGS : uint
    {
        NORMAL                  = 0x00000000,   /* Standard behaviour. */
        NONBLOCKING             = 0x00000001,   /* Bank loading occurs asynchronously rather than occurring immediately. */
        DECOMPRESS_SAMPLES      = 0x00000002,   /* Force samples to decompress into memory when they are loaded, rather than staying compressed. */
        UNENCRYPTED             = 0x00000004,   /* Ignore the encryption key specified by Studio::System::setAdvancedSettings when loading sounds from this bank. */
    }

    [Flags]
    public enum COMMANDCAPTURE_FLAGS : uint
    {
        NORMAL                  = 0x00000000,   /* Standard behaviour. */
        FILEFLUSH               = 0x00000001,   /* Call file flush on every command. */
        SKIP_INITIAL_STATE      = 0x00000002,   /* Normally the initial state of banks and instances is captured, unless this flag is set. */
    }

    [Flags]
    public enum COMMANDREPLAY_FLAGS : uint
    {
        NORMAL                  = 0x00000000,   /* Standard behaviour. */
        SKIP_CLEANUP            = 0x00000001,   /* Normally the playback will release any created resources when it stops, unless this flag is set. */
        FAST_FORWARD            = 0x00000002,   /* Play back at maximum speed, ignoring the timing of the original replay. */
        SKIP_BANK_LOAD          = 0x00000004,   /* Skip commands related to bank loading. */
    }

    public enum PLAYBACK_STATE : int
    {
        PLAYING,               /* Currently playing. */
        SUSTAINING,            /* The timeline cursor is paused on a sustain point. */
        STOPPED,               /* Not playing. */
        STARTING,              /* Start has been called but the instance is not fully started yet. */
        STOPPING,              /* Stop has been called but the instance is not fully stopped yet. */
    }

    public enum EVENT_PROPERTY : int
    {
        CHANNELPRIORITY,        /* Priority to set on low-level channels created by this event instance (-1 to 256). */
        SCHEDULE_DELAY,         /* Schedule delay to synchronized playback for multiple tracks in DS clocks, or -1 for default. */
        SCHEDULE_LOOKAHEAD,     /* Schedule look-ahead on the timeline in DSP clocks, or -1 for default. */
        MINIMUM_DISTANCE,       /* Override the event's 3D minimum distance, or -1 for default. */
        MAXIMUM_DISTANCE,       /* Override the event's 3D maximum distance, or -1 for default. */
        MAX
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct PLUGIN_INSTANCE_PROPERTIES
    {
        public IntPtr name;                           /* The name of the plugin effect or sound (set in FMOD Studio). */
        public IntPtr dsp;                            /* The DSP plugin instance. This can be cast to/from FMOD::DSP* type. */
    }

    [Flags]
    public enum EVENT_CALLBACK_TYPE : uint
    {
        CREATED                  = 0x00000001,  /* Called when an instance is fully created. Parameters = unused. */
        DESTROYED                = 0x00000002,  /* Called when an instance is just about to be destroyed. Parameters = unused. */
        STARTING                 = 0x00000004,  /* Called when an instance is preparing to start. Parameters = unused. */
        STARTED                  = 0x00000008,  /* Called when an instance starts playing. Parameters = unused. */
        RESTARTED                = 0x00000010,  /* Called when an instance is restarted. Parameters = unused. */
        STOPPED                  = 0x00000020,  /* Called when an instance stops. Parameters = unused. */
        START_FAILED             = 0x00000040,  /* Called when an instance did not start, e.g. due to polyphony. Parameters = unused. */
        CREATE_PROGRAMMER_SOUND  = 0x00000080,  /* Called when a programmer sound needs to be created in order to play a programmer instrument. Parameters = FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES. */
        DESTROY_PROGRAMMER_SOUND = 0x00000100,  /* Called when a programmer sound needs to be destroyed. Parameters = FMOD_STUDIO_PROGRAMMER_SOUND_PROPERTIES. */
        PLUGIN_CREATED           = 0x00000200,  /* Called when a DSP plugin instance has just been created. Parameters = FMOD_STUDIO_PLUGIN_INSTANCE_PROPERTIES. */
        PLUGIN_DESTROYED         = 0x00000400,  /* Called when a DSP plugin instance is about to be destroyed. Parameters = FMOD_STUDIO_PLUGIN_INSTANCE_PROPERTIES. */
        TIMELINE_MARKER          = 0x00000800,  /* Called when the timeline passes a named marker.  Parameters = FMOD_STUDIO_TIMELINE_MARKER_PROPERTIES. */
        TIMELINE_BEAT            = 0x00001000,  /* Called when the timeline hits a beat in a tempo section.  Parameters = FMOD_STUDIO_TIMELINE_BEAT_PROPERTIES. */
        SOUND_PLAYED             = 0x00002000,  /* Called when the event plays a sound.  Parameters = FMOD::Sound. */
        SOUND_STOPPED            = 0x00004000,  /* Called when the event finishes playing a sound.  Parameters = FMOD::Sound. */
        REAL_TO_VIRTUAL          = 0x00008000,  /* Called when the event becomes virtual.  Parameters = unused. */
        VIRTUAL_TO_REAL          = 0x00010000,  /* Called when the event becomes real.  Parameters = unused. */

        ALL                      = 0xFFFFFFFF,  /* Pass this mask to Studio::EventDescription::setCallback or Studio::EventInstance::setCallback to receive all callback types. */
    }

    public delegate RESULT EVENT_CALLBACK(EVENT_CALLBACK_TYPE type, EventInstance _event, IntPtr parameters);

    public delegate RESULT COMMANDREPLAY_FRAME_CALLBACK(CommandReplay replay, int commandindex, float currenttime, IntPtr userdata);
    public delegate RESULT COMMANDREPLAY_LOAD_BANK_CALLBACK(CommandReplay replay, int commandindex, Guid bankguid, StringWrapper bankfilename, LOAD_BANK_FLAGS flags, out Bank bank, IntPtr userdata);
    public delegate RESULT COMMANDREPLAY_CREATE_INSTANCE_CALLBACK(CommandReplay replay, int commandindex, EventDescription eventdescription, out EventInstance instance, IntPtr userdata);

    public enum INSTANCETYPE : int
    {
        NONE,
        SYSTEM,
        EVENTDESCRIPTION,
        EVENTINSTANCE,
        PARAMETERINSTANCE,
        BUS,
        VCA,
        BANK,
        COMMANDREPLAY,
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct COMMAND_INFO
    {
        public StringWrapper commandname;                                 /* The full name of the API function for this command. */
        public int parentcommandindex;                                     /* For commands that operate on an instance, this is the command that created the instance */
        public int framenumber;                                            /* The frame the command belongs to */
        public float frametime;                                            /* The playback time at which this command will be executed */
        public INSTANCETYPE instancetype;                                  /* The type of object that this command uses as an instance */
        public INSTANCETYPE outputtype;                                    /* The type of object that this command outputs, if any */
        public UInt32 instancehandle;                                      /* The original handle value of the instance.  This will no longer correspond to any actual object in playback. */
        public UInt32 outputhandle;                                        /* The original handle value of the command output.  This will no longer correspond to any actual object in playback. */
    }

    public struct Util
    {
        public static RESULT parseID(string idString, out Guid id)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_ParseID(encoder.byteFromStringUTF8(idString), out id);
            }
        }

        #region importfunctions
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_ParseID(byte[] idString, out Guid id);
        #endregion
    }

    public struct System
    {
        // Initialization / system functions.
        public static RESULT create(out System system)
        {
            return FMOD_Studio_System_Create(out system.handle, VERSION.number);
        }
        public RESULT setAdvancedSettings(ADVANCEDSETTINGS settings)
        {
            settings.cbsize = Marshal.SizeOf(typeof(ADVANCEDSETTINGS));
            return FMOD_Studio_System_SetAdvancedSettings(this.handle, ref settings);
        }
        public RESULT setAdvancedSettings(ADVANCEDSETTINGS settings, string encryptionKey)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                IntPtr userKey = settings.encryptionkey;
                settings.encryptionkey = encoder.intptrFromStringUTF8(encryptionKey);
                FMOD.RESULT result = setAdvancedSettings(settings);
                settings.encryptionkey = userKey;
                return result;
            }
        }
        public RESULT getAdvancedSettings(out ADVANCEDSETTINGS settings)
        {
            settings.cbsize = Marshal.SizeOf(typeof(ADVANCEDSETTINGS));
            return FMOD_Studio_System_GetAdvancedSettings(this.handle, out settings);
        }
        public RESULT initialize(int maxchannels, INITFLAGS studioflags, FMOD.INITFLAGS flags, IntPtr extradriverdata)
        {
            return FMOD_Studio_System_Initialize(this.handle, maxchannels, studioflags, flags, extradriverdata);
        }
        public RESULT release()
        {
            return FMOD_Studio_System_Release(this.handle);
        }
        public RESULT update()
        {
            return FMOD_Studio_System_Update(this.handle);
        }
        public RESULT getCoreSystem(out FMOD.System coresystem)
        {
            return FMOD_Studio_System_GetCoreSystem(this.handle, out coresystem.handle);
        }
        public RESULT getEvent(string path, out EventDescription _event)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_GetEvent(this.handle, encoder.byteFromStringUTF8(path), out _event.handle);
            }
        }
        public RESULT getBus(string path, out Bus bus)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_GetBus(this.handle, encoder.byteFromStringUTF8(path), out bus.handle);
            }
        }
        public RESULT getVCA(string path, out VCA vca)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_GetVCA(this.handle, encoder.byteFromStringUTF8(path), out vca.handle);
            }
        }
        public RESULT getBank(string path, out Bank bank)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_GetBank(this.handle, encoder.byteFromStringUTF8(path), out bank.handle);
            }
        }

        public RESULT getEventByID(Guid id, out EventDescription _event)
        {
            return FMOD_Studio_System_GetEventByID(this.handle, ref id, out _event.handle);
        }
        public RESULT getBusByID(Guid id, out Bus bus)
        {
            return FMOD_Studio_System_GetBusByID(this.handle, ref id, out bus.handle);
        }
        public RESULT getVCAByID(Guid id, out VCA vca)
        {
            return FMOD_Studio_System_GetVCAByID(this.handle, ref id, out vca.handle);
        }
        public RESULT getBankByID(Guid id, out Bank bank)
        {
            return FMOD_Studio_System_GetBankByID(this.handle, ref id, out bank.handle);
        }
        public RESULT getSoundInfo(string key, out SOUND_INFO info)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_GetSoundInfo(this.handle, encoder.byteFromStringUTF8(key), out info);
            }
        }
        public RESULT getParameterDescriptionByName(string name, out PARAMETER_DESCRIPTION parameter)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_GetParameterDescriptionByName(this.handle, encoder.byteFromStringUTF8(name), out parameter);
            }
        }
        public RESULT getParameterDescriptionByID(PARAMETER_ID id, out PARAMETER_DESCRIPTION parameter)
        {
            return FMOD_Studio_System_GetParameterDescriptionByID(this.handle, id, out parameter);
        }
        public RESULT getParameterByID(PARAMETER_ID id, out float value)
        {
            float finalValue;
            return getParameterByID(id, out value, out finalValue);
        }
        public RESULT getParameterByID(PARAMETER_ID id, out float value, out float finalvalue)
        {
            return FMOD_Studio_System_GetParameterByID(this.handle, id, out value, out finalvalue);
        }
        public RESULT setParameterByID(PARAMETER_ID id, float value, bool ignoreseekspeed = false)
        {
            return FMOD_Studio_System_SetParameterByID(this.handle, id, value, ignoreseekspeed);
        }
        public RESULT setParametersByIDs(PARAMETER_ID[] ids, float[] values, int count, bool ignoreseekspeed = false)
        {
            return FMOD_Studio_System_SetParametersByIDs(this.handle, ids, values, count, ignoreseekspeed);
        }
        public RESULT getParameterByName(string name, out float value)
        {
            float finalValue;
            return getParameterByName(name, out value, out finalValue);
        }
        public RESULT getParameterByName(string name, out float value, out float finalvalue)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_GetParameterByName(this.handle, encoder.byteFromStringUTF8(name), out value, out finalvalue);
            }
        }
        public RESULT setParameterByName(string name, float value, bool ignoreseekspeed = false)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_SetParameterByName(this.handle, encoder.byteFromStringUTF8(name), value, ignoreseekspeed);
            }
        }
        public RESULT lookupID(string path, out Guid id)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_LookupID(this.handle, encoder.byteFromStringUTF8(path), out id);
            }
        }
        public RESULT lookupPath(Guid id, out string path)
        {
            path = null;

            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                IntPtr stringMem = Marshal.AllocHGlobal(256);
                int retrieved = 0;
                RESULT result = FMOD_Studio_System_LookupPath(this.handle, ref id, stringMem, 256, out retrieved);

                if (result == RESULT.ERR_TRUNCATED)
                {
                    Marshal.FreeHGlobal(stringMem);
                    stringMem = Marshal.AllocHGlobal(retrieved);
                    result = FMOD_Studio_System_LookupPath(this.handle, ref id, stringMem, retrieved, out retrieved);
                }

                if (result == RESULT.OK)
                {
                    path = encoder.stringFromNative(stringMem);
                }
                Marshal.FreeHGlobal(stringMem);
                return result;
            }
        }
        public RESULT getNumListeners(out int numlisteners)
        {
            return FMOD_Studio_System_GetNumListeners(this.handle, out numlisteners);
        }
        public RESULT setNumListeners(int numlisteners)
        {
            return FMOD_Studio_System_SetNumListeners(this.handle, numlisteners);
        }
        public RESULT getListenerAttributes(int listener, out ATTRIBUTES_3D attributes)
        {
            return FMOD_Studio_System_GetListenerAttributes(this.handle, listener, out attributes);
        }
        public RESULT setListenerAttributes(int listener, ATTRIBUTES_3D attributes)
        {
            return FMOD_Studio_System_SetListenerAttributes(this.handle, listener, ref attributes);
        }
        public RESULT getListenerWeight(int listener, out float weight)
        {
            return FMOD_Studio_System_GetListenerWeight(this.handle, listener, out weight);
        }
        public RESULT setListenerWeight(int listener, float weight)
        {
            return FMOD_Studio_System_SetListenerWeight(this.handle, listener, weight);
        }
        public RESULT loadBankFile(string filename, LOAD_BANK_FLAGS flags, out Bank bank)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_LoadBankFile(this.handle, encoder.byteFromStringUTF8(filename), flags, out bank.handle);
            }
        }
        public RESULT loadBankMemory(byte[] buffer, LOAD_BANK_FLAGS flags, out Bank bank)
        {
            // Manually pin the byte array. It's what the marshaller should do anyway but don't leave it to chance.
            GCHandle pinnedArray = GCHandle.Alloc(buffer, GCHandleType.Pinned);
            IntPtr pointer = pinnedArray.AddrOfPinnedObject();
            RESULT result = FMOD_Studio_System_LoadBankMemory(this.handle, pointer, buffer.Length, LOAD_MEMORY_MODE.LOAD_MEMORY, flags, out bank.handle);
            pinnedArray.Free();
            return result;
        }
        public RESULT loadBankCustom(BANK_INFO info, LOAD_BANK_FLAGS flags, out Bank bank)
        {
            info.size = Marshal.SizeOf(info);
            return FMOD_Studio_System_LoadBankCustom(this.handle, ref info, flags, out bank.handle);
        }
        public RESULT unloadAll()
        {
            return FMOD_Studio_System_UnloadAll(this.handle);
        }
        public RESULT flushCommands()
        {
            return FMOD_Studio_System_FlushCommands(this.handle);
        }
        public RESULT flushSampleLoading()
        {
            return FMOD_Studio_System_FlushSampleLoading(this.handle);
        }
        public RESULT startCommandCapture(string filename, COMMANDCAPTURE_FLAGS flags)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_StartCommandCapture(this.handle, encoder.byteFromStringUTF8(filename), flags);
            }
        }
        public RESULT stopCommandCapture()
        {
            return FMOD_Studio_System_StopCommandCapture(this.handle);
        }
        public RESULT loadCommandReplay(string filename, COMMANDREPLAY_FLAGS flags, out CommandReplay replay)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_System_LoadCommandReplay(this.handle, encoder.byteFromStringUTF8(filename), flags, out replay.handle);
            }
        }
        public RESULT getBankCount(out int count)
        {
            return FMOD_Studio_System_GetBankCount(this.handle, out count);
        }
        public RESULT getBankList(out Bank[] array)
        {
            array = null;

            RESULT result;
            int capacity;
            result = FMOD_Studio_System_GetBankCount(this.handle, out capacity);
            if (result != RESULT.OK)
            {
                return result;
            }
            if (capacity == 0)
            {
                array = new Bank[0];
                return result;
            }

            IntPtr[] rawArray = new IntPtr[capacity];
            int actualCount;
            result = FMOD_Studio_System_GetBankList(this.handle, rawArray, capacity, out actualCount);
            if (result != RESULT.OK)
            {
                return result;
            }
            if (actualCount > capacity) // More items added since we queried just now?
            {
                actualCount = capacity;
            }
            array = new Bank[actualCount];
            for (int i = 0; i < actualCount; ++i)
            {
                array[i].handle = rawArray[i];
            }
            return RESULT.OK;
        }
        public RESULT getParameterDescriptionCount(out int count)
        {
            return FMOD_Studio_System_GetParameterDescriptionCount(this.handle, out count);
        }
        public RESULT getParameterDescriptionList(out PARAMETER_DESCRIPTION[] array)
        {
            array = null;

            int capacity;
            RESULT result = FMOD_Studio_System_GetParameterDescriptionCount(this.handle, out capacity);
            if (result != RESULT.OK)
            {
                return result;
            }
            if (capacity == 0)
            {
                array = new PARAMETER_DESCRIPTION[0];
                return RESULT.OK;
            }

            PARAMETER_DESCRIPTION[] tempArray = new PARAMETER_DESCRIPTION[capacity];
            int actualCount;
            result = FMOD_Studio_System_GetParameterDescriptionList(this.handle, tempArray, capacity, out actualCount);
            if (result != RESULT.OK)
            {
                return result;
            }

            if (actualCount != capacity)
            {
                Array.Resize(ref tempArray, actualCount);
            }

            array = tempArray;

            return RESULT.OK;
        }
        public RESULT getCPUUsage(out CPU_USAGE usage)
        {
            return FMOD_Studio_System_GetCPUUsage(this.handle, out usage);
        }
        public RESULT getBufferUsage(out BUFFER_USAGE usage)
        {
            return FMOD_Studio_System_GetBufferUsage(this.handle, out usage);
        }
        public RESULT resetBufferUsage()
        {
            return FMOD_Studio_System_ResetBufferUsage(this.handle);
        }

        public RESULT setCallback(SYSTEM_CALLBACK callback, SYSTEM_CALLBACK_TYPE callbackmask = SYSTEM_CALLBACK_TYPE.ALL)
        {
            return FMOD_Studio_System_SetCallback(this.handle, callback, callbackmask);
        }

        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD_Studio_System_GetUserData(this.handle, out userdata);
        }

        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD_Studio_System_SetUserData(this.handle, userdata);
        }

        #region importfunctions
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_Create                  (out IntPtr system, uint headerversion);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern bool   FMOD_Studio_System_IsValid                 (IntPtr system);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_SetAdvancedSettings     (IntPtr system, ref ADVANCEDSETTINGS settings);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetAdvancedSettings     (IntPtr system, out ADVANCEDSETTINGS settings);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_Initialize              (IntPtr system, int maxchannels, INITFLAGS studioflags, FMOD.INITFLAGS flags, IntPtr extradriverdata);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_Release                 (IntPtr system);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_Update                  (IntPtr system);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetCoreSystem           (IntPtr system, out IntPtr coresystem);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetEvent                (IntPtr system, byte[] path, out IntPtr _event);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetBus                  (IntPtr system, byte[] path, out IntPtr bus);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetVCA                  (IntPtr system, byte[] path, out IntPtr vca);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetBank                 (IntPtr system, byte[] path, out IntPtr bank);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetEventByID            (IntPtr system, ref Guid id, out IntPtr _event);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetBusByID              (IntPtr system, ref Guid id, out IntPtr bus);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetVCAByID              (IntPtr system, ref Guid id, out IntPtr vca);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetBankByID             (IntPtr system, ref Guid id, out IntPtr bank);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetSoundInfo            (IntPtr system, byte[] key, out SOUND_INFO info);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetParameterDescriptionByName(IntPtr system, byte[] name, out PARAMETER_DESCRIPTION parameter);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetParameterDescriptionByID(IntPtr system, PARAMETER_ID id, out PARAMETER_DESCRIPTION parameter);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetParameterByID        (IntPtr system, PARAMETER_ID id, out float value, out float finalvalue);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_SetParameterByID        (IntPtr system, PARAMETER_ID id, float value, bool ignoreseekspeed);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_SetParametersByIDs      (IntPtr system, PARAMETER_ID[] ids, float[] values, int count, bool ignoreseekspeed);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetParameterByName      (IntPtr system, byte[] name, out float value, out float finalvalue);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_SetParameterByName      (IntPtr system, byte[] name, float value, bool ignoreseekspeed);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_LookupID                (IntPtr system, byte[] path, out Guid id);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_LookupPath              (IntPtr system, ref Guid id, IntPtr path, int size, out int retrieved);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetNumListeners         (IntPtr system, out int numlisteners);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_SetNumListeners         (IntPtr system, int numlisteners);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetListenerAttributes   (IntPtr system, int listener, out ATTRIBUTES_3D attributes);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_SetListenerAttributes   (IntPtr system, int listener, ref ATTRIBUTES_3D attributes);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetListenerWeight       (IntPtr system, int listener, out float weight);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_SetListenerWeight       (IntPtr system, int listener, float weight);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_LoadBankFile            (IntPtr system, byte[] filename, LOAD_BANK_FLAGS flags, out IntPtr bank);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_LoadBankMemory          (IntPtr system, IntPtr buffer, int length, LOAD_MEMORY_MODE mode, LOAD_BANK_FLAGS flags, out IntPtr bank);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_LoadBankCustom          (IntPtr system, ref BANK_INFO info, LOAD_BANK_FLAGS flags, out IntPtr bank);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_UnloadAll               (IntPtr system);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_FlushCommands           (IntPtr system);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_FlushSampleLoading      (IntPtr system);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_StartCommandCapture     (IntPtr system, byte[] filename, COMMANDCAPTURE_FLAGS flags);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_StopCommandCapture      (IntPtr system);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_LoadCommandReplay       (IntPtr system, byte[] filename, COMMANDREPLAY_FLAGS flags, out IntPtr replay);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetBankCount            (IntPtr system, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetBankList             (IntPtr system, IntPtr[] array, int capacity, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetParameterDescriptionCount(IntPtr system, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetParameterDescriptionList(IntPtr system, [Out] PARAMETER_DESCRIPTION[] array, int capacity, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetCPUUsage             (IntPtr system, out CPU_USAGE usage);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetBufferUsage          (IntPtr system, out BUFFER_USAGE usage);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_ResetBufferUsage        (IntPtr system);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_SetCallback             (IntPtr system, SYSTEM_CALLBACK callback, SYSTEM_CALLBACK_TYPE callbackmask);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_GetUserData             (IntPtr system, out IntPtr userdata);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_System_SetUserData             (IntPtr system, IntPtr userdata);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        public bool isValid()
        {
            return hasHandle() && FMOD_Studio_System_IsValid(this.handle);
        }

        #endregion
    }

    public struct EventDescription
    {
        public RESULT getID(out Guid id)
        {
            return FMOD_Studio_EventDescription_GetID(this.handle, out id);
        }
        public RESULT getPath(out string path)
        {
            path = null;

            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                IntPtr stringMem = Marshal.AllocHGlobal(256);
                int retrieved = 0;
                RESULT result = FMOD_Studio_EventDescription_GetPath(this.handle, stringMem, 256, out retrieved);

                if (result == RESULT.ERR_TRUNCATED)
                {
                    Marshal.FreeHGlobal(stringMem);
                    stringMem = Marshal.AllocHGlobal(retrieved);
                    result = FMOD_Studio_EventDescription_GetPath(this.handle, stringMem, retrieved, out retrieved);
                }

                if (result == RESULT.OK)
                {
                    path = encoder.stringFromNative(stringMem);
                }
                Marshal.FreeHGlobal(stringMem);
                return result;
            }
        }
        public RESULT getParameterDescriptionCount(out int count)
        {
            return FMOD_Studio_EventDescription_GetParameterDescriptionCount(this.handle, out count);
        }
        public RESULT getParameterDescriptionByIndex(int index, out PARAMETER_DESCRIPTION parameter)
        {
            return FMOD_Studio_EventDescription_GetParameterDescriptionByIndex(this.handle, index, out parameter);
        }
        public RESULT getParameterDescriptionByName(string name, out PARAMETER_DESCRIPTION parameter)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_EventDescription_GetParameterDescriptionByName(this.handle, encoder.byteFromStringUTF8(name), out parameter);
            }
        }
        public RESULT getParameterDescriptionByID(PARAMETER_ID id, out PARAMETER_DESCRIPTION parameter)
        {
            return FMOD_Studio_EventDescription_GetParameterDescriptionByID(this.handle, id, out parameter);
        }
        public RESULT getUserPropertyCount(out int count)
        {
            return FMOD_Studio_EventDescription_GetUserPropertyCount(this.handle, out count);
        }
        public RESULT getUserPropertyByIndex(int index, out USER_PROPERTY property)
        {
            return FMOD_Studio_EventDescription_GetUserPropertyByIndex(this.handle, index, out property);
        }
        public RESULT getUserProperty(string name, out USER_PROPERTY property)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_EventDescription_GetUserProperty(this.handle, encoder.byteFromStringUTF8(name), out property);
            }
        }
        public RESULT getLength(out int length)
        {
            return FMOD_Studio_EventDescription_GetLength(this.handle, out length);
        }
        public RESULT getMinimumDistance(out float distance)
        {
            return FMOD_Studio_EventDescription_GetMinimumDistance(this.handle, out distance);
        }
        public RESULT getMaximumDistance(out float distance)
        {
            return FMOD_Studio_EventDescription_GetMaximumDistance(this.handle, out distance);
        }
        public RESULT getSoundSize(out float size)
        {
            return FMOD_Studio_EventDescription_GetSoundSize(this.handle, out size);
        }
        public RESULT isSnapshot(out bool snapshot)
        {
            return FMOD_Studio_EventDescription_IsSnapshot(this.handle, out snapshot);
        }
        public RESULT isOneshot(out bool oneshot)
        {
            return FMOD_Studio_EventDescription_IsOneshot(this.handle, out oneshot);
        }
        public RESULT isStream(out bool isStream)
        {
            return FMOD_Studio_EventDescription_IsStream(this.handle, out isStream);
        }
        public RESULT is3D(out bool is3D)
        {
            return FMOD_Studio_EventDescription_Is3D(this.handle, out is3D);
        }
        public RESULT hasCue(out bool cue)
        {
            return FMOD_Studio_EventDescription_HasCue(this.handle, out cue);
        }

        public RESULT createInstance(out EventInstance instance)
        {
            return FMOD_Studio_EventDescription_CreateInstance(this.handle, out instance.handle);
        }

        public RESULT getInstanceCount(out int count)
        {
            return FMOD_Studio_EventDescription_GetInstanceCount(this.handle, out count);
        }
        public RESULT getInstanceList(out EventInstance[] array)
        {
            array = null;

            RESULT result;
            int capacity;
            result = FMOD_Studio_EventDescription_GetInstanceCount(this.handle, out capacity);
            if (result != RESULT.OK)
            {
                return result;
            }
            if (capacity == 0)
            {
                array = new EventInstance[0];
                return result;
            }

            IntPtr[] rawArray = new IntPtr[capacity];
            int actualCount;
            result = FMOD_Studio_EventDescription_GetInstanceList(this.handle, rawArray, capacity, out actualCount);
            if (result != RESULT.OK)
            {
                return result;
            }
            if (actualCount > capacity) // More items added since we queried just now?
            {
                actualCount = capacity;
            }
            array = new EventInstance[actualCount];
            for (int i = 0; i < actualCount; ++i)
            {
                array[i].handle = rawArray[i];
            }
            return RESULT.OK;
        }

        public RESULT loadSampleData()
        {
            return FMOD_Studio_EventDescription_LoadSampleData(this.handle);
        }

        public RESULT unloadSampleData()
        {
            return FMOD_Studio_EventDescription_UnloadSampleData(this.handle);
        }

        public RESULT getSampleLoadingState(out LOADING_STATE state)
        {
            return FMOD_Studio_EventDescription_GetSampleLoadingState(this.handle, out state);
        }

        public RESULT releaseAllInstances()
        {
            return FMOD_Studio_EventDescription_ReleaseAllInstances(this.handle);
        }
        public RESULT setCallback(EVENT_CALLBACK callback, EVENT_CALLBACK_TYPE callbackmask = EVENT_CALLBACK_TYPE.ALL)
        {
            return FMOD_Studio_EventDescription_SetCallback(this.handle, callback, callbackmask);
        }

        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD_Studio_EventDescription_GetUserData(this.handle, out userdata);
        }

        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD_Studio_EventDescription_SetUserData(this.handle, userdata);
        }

        #region importfunctions
        [DllImport(STUDIO_VERSION.dll)]
        private static extern bool FMOD_Studio_EventDescription_IsValid                 (IntPtr eventdescription);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetID                 (IntPtr eventdescription, out Guid id);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetPath               (IntPtr eventdescription, IntPtr path, int size, out int retrieved);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetParameterDescriptionCount(IntPtr eventdescription, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetParameterDescriptionByIndex(IntPtr eventdescription, int index, out PARAMETER_DESCRIPTION parameter);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetParameterDescriptionByName(IntPtr eventdescription, byte[] name, out PARAMETER_DESCRIPTION parameter);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetParameterDescriptionByID(IntPtr eventdescription, PARAMETER_ID id, out PARAMETER_DESCRIPTION parameter);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetUserPropertyCount  (IntPtr eventdescription, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetUserPropertyByIndex(IntPtr eventdescription, int index, out USER_PROPERTY property);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetUserProperty       (IntPtr eventdescription, byte[] name, out USER_PROPERTY property);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetLength             (IntPtr eventdescription, out int length);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetMinimumDistance    (IntPtr eventdescription, out float distance);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetMaximumDistance    (IntPtr eventdescription, out float distance);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetSoundSize          (IntPtr eventdescription, out float size);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_IsSnapshot            (IntPtr eventdescription, out bool snapshot);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_IsOneshot             (IntPtr eventdescription, out bool oneshot);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_IsStream              (IntPtr eventdescription, out bool isStream);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_Is3D                  (IntPtr eventdescription, out bool is3D);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_HasCue                (IntPtr eventdescription, out bool cue);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_CreateInstance        (IntPtr eventdescription, out IntPtr instance);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetInstanceCount      (IntPtr eventdescription, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetInstanceList       (IntPtr eventdescription, IntPtr[] array, int capacity, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_LoadSampleData        (IntPtr eventdescription);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_UnloadSampleData      (IntPtr eventdescription);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetSampleLoadingState (IntPtr eventdescription, out LOADING_STATE state);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_ReleaseAllInstances   (IntPtr eventdescription);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_SetCallback           (IntPtr eventdescription, EVENT_CALLBACK callback, EVENT_CALLBACK_TYPE callbackmask);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_GetUserData           (IntPtr eventdescription, out IntPtr userdata);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventDescription_SetUserData           (IntPtr eventdescription, IntPtr userdata);
        #endregion
        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        public bool isValid()
        {
            return hasHandle() && FMOD_Studio_EventDescription_IsValid(this.handle);
        }

        #endregion
    }

    public struct EventInstance
    {
        public RESULT getDescription(out EventDescription description)
        {
            return FMOD_Studio_EventInstance_GetDescription(this.handle, out description.handle);
        }
        public RESULT getVolume(out float volume)
        {
            float finalVolume;
            return getVolume(out volume, out finalVolume);
        }
        public RESULT getVolume(out float volume, out float finalvolume)
        {
            return FMOD_Studio_EventInstance_GetVolume(this.handle, out volume, out finalvolume);
        }
        public RESULT setVolume(float volume)
        {
            return FMOD_Studio_EventInstance_SetVolume(this.handle, volume);
        }
        public RESULT getPitch(out float pitch)
        {
            float finalPitch;
            return getPitch(out pitch, out finalPitch);
        }
        public RESULT getPitch(out float pitch, out float finalpitch)
        {
            return FMOD_Studio_EventInstance_GetPitch(this.handle, out pitch, out finalpitch);
        }
        public RESULT setPitch(float pitch)
        {
            return FMOD_Studio_EventInstance_SetPitch(this.handle, pitch);
        }
        public RESULT get3DAttributes(out ATTRIBUTES_3D attributes)
        {
            return FMOD_Studio_EventInstance_Get3DAttributes(this.handle, out attributes);
        }
        public RESULT set3DAttributes(ATTRIBUTES_3D attributes)
        {
            return FMOD_Studio_EventInstance_Set3DAttributes(this.handle, ref attributes);
        }
        public RESULT getListenerMask(out uint mask)
        {
            return FMOD_Studio_EventInstance_GetListenerMask(this.handle, out mask);
        }
        public RESULT setListenerMask(uint mask)
        {
            return FMOD_Studio_EventInstance_SetListenerMask(this.handle, mask);
        }
        public RESULT getProperty(EVENT_PROPERTY index, out float value)
        {
            return FMOD_Studio_EventInstance_GetProperty(this.handle, index, out value);
        }
        public RESULT setProperty(EVENT_PROPERTY index, float value)
        {
            return FMOD_Studio_EventInstance_SetProperty(this.handle, index, value);
        }
        public RESULT getReverbLevel(int index, out float level)
        {
            return FMOD_Studio_EventInstance_GetReverbLevel(this.handle, index, out level);
        }
        public RESULT setReverbLevel(int index, float level)
        {
            return FMOD_Studio_EventInstance_SetReverbLevel(this.handle, index, level);
        }
        public RESULT getPaused(out bool paused)
        {
            return FMOD_Studio_EventInstance_GetPaused(this.handle, out paused);
        }
        public RESULT setPaused(bool paused)
        {
            return FMOD_Studio_EventInstance_SetPaused(this.handle, paused);
        }
        public RESULT start()
        {
            return FMOD_Studio_EventInstance_Start(this.handle);
        }
        public RESULT stop(STOP_MODE mode)
        {
            return FMOD_Studio_EventInstance_Stop(this.handle, mode);
        }
        public RESULT getTimelinePosition(out int position)
        {
            return FMOD_Studio_EventInstance_GetTimelinePosition(this.handle, out position);
        }
        public RESULT setTimelinePosition(int position)
        {
            return FMOD_Studio_EventInstance_SetTimelinePosition(this.handle, position);
        }
        public RESULT getPlaybackState(out PLAYBACK_STATE state)
        {
            return FMOD_Studio_EventInstance_GetPlaybackState(this.handle, out state);
        }
        public RESULT getChannelGroup(out FMOD.ChannelGroup group)
        {
            return FMOD_Studio_EventInstance_GetChannelGroup(this.handle, out group.handle);
        }
        public RESULT release()
        {
            return FMOD_Studio_EventInstance_Release(this.handle);
        }
        public RESULT isVirtual(out bool virtualstate)
        {
            return FMOD_Studio_EventInstance_IsVirtual(this.handle, out virtualstate);
        }
        public RESULT getParameterByID(PARAMETER_ID id, out float value)
        {
            float finalvalue;
            return getParameterByID(id, out value, out finalvalue);
        }
        public RESULT getParameterByID(PARAMETER_ID id, out float value, out float finalvalue)
        {
            return FMOD_Studio_EventInstance_GetParameterByID(this.handle, id, out value, out finalvalue);
        }
        public RESULT setParameterByID(PARAMETER_ID id, float value, bool ignoreseekspeed = false)
        {
            return FMOD_Studio_EventInstance_SetParameterByID(this.handle, id, value, ignoreseekspeed);
        }
        public RESULT setParametersByIDs(PARAMETER_ID[] ids, float[] values, int count, bool ignoreseekspeed = false)
        {
            return FMOD_Studio_EventInstance_SetParametersByIDs(this.handle, ids, values, count, ignoreseekspeed);
        }
        public RESULT getParameterByName(string name, out float value)
        {
            float finalValue;
            return getParameterByName(name, out value, out finalValue);
        }
        public RESULT getParameterByName(string name, out float value, out float finalvalue)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_EventInstance_GetParameterByName(this.handle, encoder.byteFromStringUTF8(name), out value, out finalvalue);
            }
        }
        public RESULT setParameterByName(string name, float value, bool ignoreseekspeed = false)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_EventInstance_SetParameterByName(this.handle, encoder.byteFromStringUTF8(name), value, ignoreseekspeed);
            }
        }
        public RESULT triggerCue()
        {
            return FMOD_Studio_EventInstance_TriggerCue(this.handle);
        }
        public RESULT setCallback(EVENT_CALLBACK callback, EVENT_CALLBACK_TYPE callbackmask = EVENT_CALLBACK_TYPE.ALL)
        {
            return FMOD_Studio_EventInstance_SetCallback(this.handle, callback, callbackmask);
        }
        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD_Studio_EventInstance_GetUserData(this.handle, out userdata);
        }
        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD_Studio_EventInstance_SetUserData(this.handle, userdata);
        }
        public RESULT getCPUUsage(out uint exclusive, out uint inclusive)
        {
            return FMOD_Studio_EventInstance_GetCPUUsage(this.handle, out exclusive, out inclusive);
        }

        #region importfunctions
        [DllImport(STUDIO_VERSION.dll)]
        private static extern bool   FMOD_Studio_EventInstance_IsValid                     (IntPtr _event);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetDescription              (IntPtr _event, out IntPtr description);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetVolume                   (IntPtr _event, out float volume, out float finalvolume);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetVolume                   (IntPtr _event, float volume);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetPitch                    (IntPtr _event, out float pitch, out float finalpitch);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetPitch                    (IntPtr _event, float pitch);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_Get3DAttributes             (IntPtr _event, out ATTRIBUTES_3D attributes);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_Set3DAttributes             (IntPtr _event, ref ATTRIBUTES_3D attributes);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetListenerMask             (IntPtr _event, out uint mask);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetListenerMask             (IntPtr _event, uint mask);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetProperty                 (IntPtr _event, EVENT_PROPERTY index, out float value);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetProperty                 (IntPtr _event, EVENT_PROPERTY index, float value);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetReverbLevel              (IntPtr _event, int index, out float level);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetReverbLevel              (IntPtr _event, int index, float level);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetPaused                   (IntPtr _event, out bool paused);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetPaused                   (IntPtr _event, bool paused);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_Start                       (IntPtr _event);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_Stop                        (IntPtr _event, STOP_MODE mode);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetTimelinePosition         (IntPtr _event, out int position);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetTimelinePosition         (IntPtr _event, int position);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetPlaybackState            (IntPtr _event, out PLAYBACK_STATE state);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetChannelGroup             (IntPtr _event, out IntPtr group);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_Release                     (IntPtr _event);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_IsVirtual                   (IntPtr _event, out bool virtualstate);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetParameterByName          (IntPtr _event, byte[] name, out float value, out float finalvalue);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetParameterByName          (IntPtr _event, byte[] name, float value, bool ignoreseekspeed);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetParameterByID            (IntPtr _event, PARAMETER_ID id, out float value, out float finalvalue);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetParameterByID            (IntPtr _event, PARAMETER_ID id, float value, bool ignoreseekspeed);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetParametersByIDs          (IntPtr _event, PARAMETER_ID[] ids, float[] values, int count, bool ignoreseekspeed);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_TriggerCue                  (IntPtr _event);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetCallback                 (IntPtr _event, EVENT_CALLBACK callback, EVENT_CALLBACK_TYPE callbackmask);
        [DllImport (STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetUserData                 (IntPtr _event, out IntPtr userdata);
        [DllImport (STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_SetUserData                 (IntPtr _event, IntPtr userdata);
        [DllImport (STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_EventInstance_GetCPUUsage                 (IntPtr _event, out uint exclusive, out uint inclusive);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        public bool isValid()
        {
            return hasHandle() && FMOD_Studio_EventInstance_IsValid(this.handle);
        }

        #endregion
    }

    public struct Bus
    {
        public RESULT getID(out Guid id)
        {
            return FMOD_Studio_Bus_GetID(this.handle, out id);
        }
        public RESULT getPath(out string path)
        {
            path = null;

            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                IntPtr stringMem = Marshal.AllocHGlobal(256);
                int retrieved = 0;
                RESULT result = FMOD_Studio_Bus_GetPath(this.handle, stringMem, 256, out retrieved);

                if (result == RESULT.ERR_TRUNCATED)
                {
                    Marshal.FreeHGlobal(stringMem);
                    stringMem = Marshal.AllocHGlobal(retrieved);
                    result = FMOD_Studio_Bus_GetPath(this.handle, stringMem, retrieved, out retrieved);
                }

                if (result == RESULT.OK)
                {
                    path = encoder.stringFromNative(stringMem);
                }
                Marshal.FreeHGlobal(stringMem);
                return result;
            }

        }
        public RESULT getVolume(out float volume)
        {
            float finalVolume;
            return getVolume(out volume, out finalVolume);
        }
        public RESULT getVolume(out float volume, out float finalvolume)
        {
            return FMOD_Studio_Bus_GetVolume(this.handle, out volume, out finalvolume);
        }
        public RESULT setVolume(float volume)
        {
            return FMOD_Studio_Bus_SetVolume(this.handle, volume);
        }
        public RESULT getPaused(out bool paused)
        {
            return FMOD_Studio_Bus_GetPaused(this.handle, out paused);
        }
        public RESULT setPaused(bool paused)
        {
            return FMOD_Studio_Bus_SetPaused(this.handle, paused);
        }
        public RESULT getMute(out bool mute)
        {
            return FMOD_Studio_Bus_GetMute(this.handle, out mute);
        }
        public RESULT setMute(bool mute)
        {
            return FMOD_Studio_Bus_SetMute(this.handle, mute);
        }
        public RESULT stopAllEvents(STOP_MODE mode)
        {
            return FMOD_Studio_Bus_StopAllEvents(this.handle, mode);
        }
        public RESULT lockChannelGroup()
        {
            return FMOD_Studio_Bus_LockChannelGroup(this.handle);
        }
        public RESULT unlockChannelGroup()
        {
            return FMOD_Studio_Bus_UnlockChannelGroup(this.handle);
        }
        public RESULT getChannelGroup(out FMOD.ChannelGroup group)
        {
            return FMOD_Studio_Bus_GetChannelGroup(this.handle, out group.handle);
        }
        public RESULT getCPUUsage(out uint exclusive, out uint inclusive)
        {
            return FMOD_Studio_Bus_GetCPUUsage(this.handle, out exclusive, out inclusive);
        }

        #region importfunctions
        [DllImport(STUDIO_VERSION.dll)]
        private static extern bool   FMOD_Studio_Bus_IsValid              (IntPtr bus);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_GetID                (IntPtr bus, out Guid id);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_GetPath              (IntPtr bus, IntPtr path, int size, out int retrieved);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_GetVolume            (IntPtr bus, out float volume, out float finalvolume);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_SetVolume            (IntPtr bus, float volume);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_GetPaused            (IntPtr bus, out bool paused);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_SetPaused            (IntPtr bus, bool paused);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_GetMute              (IntPtr bus, out bool mute);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_SetMute              (IntPtr bus, bool mute);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_StopAllEvents        (IntPtr bus, STOP_MODE mode);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_LockChannelGroup     (IntPtr bus);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_UnlockChannelGroup   (IntPtr bus);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_GetChannelGroup      (IntPtr bus, out IntPtr group);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bus_GetCPUUsage          (IntPtr bus, out uint exclusive, out uint inclusive);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        public bool isValid()
        {
            return hasHandle() && FMOD_Studio_Bus_IsValid(this.handle);
        }

        #endregion
    }

    public struct VCA
    {
        public RESULT getID(out Guid id)
        {
            return FMOD_Studio_VCA_GetID(this.handle, out id);
        }
        public RESULT getPath(out string path)
        {
            path = null;

            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                IntPtr stringMem = Marshal.AllocHGlobal(256);
                int retrieved = 0;
                RESULT result = FMOD_Studio_VCA_GetPath(this.handle, stringMem, 256, out retrieved);

                if (result == RESULT.ERR_TRUNCATED)
                {
                    Marshal.FreeHGlobal(stringMem);
                    stringMem = Marshal.AllocHGlobal(retrieved);
                    result = FMOD_Studio_VCA_GetPath(this.handle, stringMem, retrieved, out retrieved);
                }

                if (result == RESULT.OK)
                {
                    path = encoder.stringFromNative(stringMem);
                }
                Marshal.FreeHGlobal(stringMem);
                return result;
            }
        }
        public RESULT getVolume(out float volume)
        {
            float finalVolume;
            return getVolume(out volume, out finalVolume);
        }
        public RESULT getVolume(out float volume, out float finalvolume)
        {
            return FMOD_Studio_VCA_GetVolume(this.handle, out volume, out finalvolume);
        }
        public RESULT setVolume(float volume)
        {
            return FMOD_Studio_VCA_SetVolume(this.handle, volume);
        }

        #region importfunctions
        [DllImport(STUDIO_VERSION.dll)]
        private static extern bool   FMOD_Studio_VCA_IsValid       (IntPtr vca);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_VCA_GetID         (IntPtr vca, out Guid id);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_VCA_GetPath       (IntPtr vca, IntPtr path, int size, out int retrieved);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_VCA_GetVolume     (IntPtr vca, out float volume, out float finalvolume);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_VCA_SetVolume     (IntPtr vca, float volume);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        public bool isValid()
        {
            return hasHandle() && FMOD_Studio_VCA_IsValid(this.handle);
        }

        #endregion
    }

    public struct Bank
    {
        // Property access

        public RESULT getID(out Guid id)
        {
            return FMOD_Studio_Bank_GetID(this.handle, out id);
        }
        public RESULT getPath(out string path)
        {
            path = null;

            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                IntPtr stringMem = Marshal.AllocHGlobal(256);
                int retrieved = 0;
                RESULT result = FMOD_Studio_Bank_GetPath(this.handle, stringMem, 256, out retrieved);

                if (result == RESULT.ERR_TRUNCATED)
                {
                    Marshal.FreeHGlobal(stringMem);
                    stringMem = Marshal.AllocHGlobal(retrieved);
                    result = FMOD_Studio_Bank_GetPath(this.handle, stringMem, retrieved, out retrieved);
                }

                if (result == RESULT.OK)
                {
                    path = encoder.stringFromNative(stringMem);
                }
                Marshal.FreeHGlobal(stringMem);
                return result;
            }
        }
        public RESULT unload()
        {
            return FMOD_Studio_Bank_Unload(this.handle);
        }
        public RESULT loadSampleData()
        {
            return FMOD_Studio_Bank_LoadSampleData(this.handle);
        }
        public RESULT unloadSampleData()
        {
            return FMOD_Studio_Bank_UnloadSampleData(this.handle);
        }
        public RESULT getLoadingState(out LOADING_STATE state)
        {
            return FMOD_Studio_Bank_GetLoadingState(this.handle, out state);
        }
        public RESULT getSampleLoadingState(out LOADING_STATE state)
        {
            return FMOD_Studio_Bank_GetSampleLoadingState(this.handle, out state);
        }

        // Enumeration
        public RESULT getStringCount(out int count)
        {
            return FMOD_Studio_Bank_GetStringCount(this.handle, out count);
        }
        public RESULT getStringInfo(int index, out Guid id, out string path)
        {
            path = null;
            id = Guid.Empty;

            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                IntPtr stringMem = Marshal.AllocHGlobal(256);
                int retrieved = 0;
                RESULT result = FMOD_Studio_Bank_GetStringInfo(this.handle, index, out id, stringMem, 256, out retrieved);

                if (result == RESULT.ERR_TRUNCATED)
                {
                    Marshal.FreeHGlobal(stringMem);
                    stringMem = Marshal.AllocHGlobal(retrieved);
                    result = FMOD_Studio_Bank_GetStringInfo(this.handle, index, out id, stringMem, retrieved, out retrieved);
                }

                if (result == RESULT.OK)
                {
                    path = encoder.stringFromNative(stringMem);
                }
                Marshal.FreeHGlobal(stringMem);
                return result;
            }
        }

        public RESULT getEventCount(out int count)
        {
            return FMOD_Studio_Bank_GetEventCount(this.handle, out count);
        }
        public RESULT getEventList(out EventDescription[] array)
        {
            array = null;

            RESULT result;
            int capacity;
            result = FMOD_Studio_Bank_GetEventCount(this.handle, out capacity);
            if (result != RESULT.OK)
            {
                return result;
            }
            if (capacity == 0)
            {
                array = new EventDescription[0];
                return result;
            }

            IntPtr[] rawArray = new IntPtr[capacity];
            int actualCount;
            result = FMOD_Studio_Bank_GetEventList(this.handle, rawArray, capacity, out actualCount);
            if (result != RESULT.OK)
            {
                return result;
            }
            if (actualCount > capacity) // More items added since we queried just now?
            {
                actualCount = capacity;
            }
            array = new EventDescription[actualCount];
            for (int i = 0; i < actualCount; ++i)
            {
                array[i].handle = rawArray[i];
            }
            return RESULT.OK;
        }
        public RESULT getBusCount(out int count)
        {
            return FMOD_Studio_Bank_GetBusCount(this.handle, out count);
        }
        public RESULT getBusList(out Bus[] array)
        {
            array = null;

            RESULT result;
            int capacity;
            result = FMOD_Studio_Bank_GetBusCount(this.handle, out capacity);
            if (result != RESULT.OK)
            {
                return result;
            }
            if (capacity == 0)
            {
                array = new Bus[0];
                return result;
            }

            IntPtr[] rawArray = new IntPtr[capacity];
            int actualCount;
            result = FMOD_Studio_Bank_GetBusList(this.handle, rawArray, capacity, out actualCount);
            if (result != RESULT.OK)
            {
                return result;
            }
            if (actualCount > capacity) // More items added since we queried just now?
            {
                actualCount = capacity;
            }
            array = new Bus[actualCount];
            for (int i = 0; i < actualCount; ++i)
            {
                array[i].handle = rawArray[i];
            }
            return RESULT.OK;
        }
        public RESULT getVCACount(out int count)
        {
            return FMOD_Studio_Bank_GetVCACount(this.handle, out count);
        }
        public RESULT getVCAList(out VCA[] array)
        {
            array = null;

            RESULT result;
            int capacity;
            result = FMOD_Studio_Bank_GetVCACount(this.handle, out capacity);
            if (result != RESULT.OK)
            {
                return result;
            }
            if (capacity == 0)
            {
                array = new VCA[0];
                return result;
            }

            IntPtr[] rawArray = new IntPtr[capacity];
            int actualCount;
            result = FMOD_Studio_Bank_GetVCAList(this.handle, rawArray, capacity, out actualCount);
            if (result != RESULT.OK)
            {
                return result;
            }
            if (actualCount > capacity) // More items added since we queried just now?
            {
                actualCount = capacity;
            }
            array = new VCA[actualCount];
            for (int i = 0; i < actualCount; ++i)
            {
                array[i].handle = rawArray[i];
            }
            return RESULT.OK;
        }

        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD_Studio_Bank_GetUserData(this.handle, out userdata);
        }

        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD_Studio_Bank_SetUserData(this.handle, userdata);
        }

        #region importfunctions
        [DllImport(STUDIO_VERSION.dll)]
        private static extern bool   FMOD_Studio_Bank_IsValid                   (IntPtr bank);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetID                     (IntPtr bank, out Guid id);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetPath                   (IntPtr bank, IntPtr path, int size, out int retrieved);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_Unload                    (IntPtr bank);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_LoadSampleData            (IntPtr bank);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_UnloadSampleData          (IntPtr bank);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetLoadingState           (IntPtr bank, out LOADING_STATE state);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetSampleLoadingState     (IntPtr bank, out LOADING_STATE state);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetStringCount            (IntPtr bank, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetStringInfo             (IntPtr bank, int index, out Guid id, IntPtr path, int size, out int retrieved);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetEventCount             (IntPtr bank, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetEventList              (IntPtr bank, IntPtr[] array, int capacity, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetBusCount               (IntPtr bank, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetBusList                (IntPtr bank, IntPtr[] array, int capacity, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetVCACount               (IntPtr bank, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetVCAList                (IntPtr bank, IntPtr[] array, int capacity, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_GetUserData               (IntPtr bank, out IntPtr userdata);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_Bank_SetUserData               (IntPtr bank, IntPtr userdata);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        public bool isValid()
        {
            return hasHandle() && FMOD_Studio_Bank_IsValid(this.handle);
        }

        #endregion
    }

    public struct CommandReplay
    {
        // Information query
        public RESULT getSystem(out System system)
        {
            return FMOD_Studio_CommandReplay_GetSystem(this.handle, out system.handle);
        }

        public RESULT getLength(out float length)
        {
            return FMOD_Studio_CommandReplay_GetLength(this.handle, out length);
        }
        public RESULT getCommandCount(out int count)
        {
            return FMOD_Studio_CommandReplay_GetCommandCount(this.handle, out count);
        }
        public RESULT getCommandInfo(int commandIndex, out COMMAND_INFO info)
        {
            return FMOD_Studio_CommandReplay_GetCommandInfo(this.handle, commandIndex, out info);
        }

        public RESULT getCommandString(int commandIndex, out string buffer)
        {
            buffer = null;
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                int stringLength = 256;
                IntPtr stringMem = Marshal.AllocHGlobal(256);
                RESULT result = FMOD_Studio_CommandReplay_GetCommandString(this.handle, commandIndex, stringMem, stringLength);

                while (result == RESULT.ERR_TRUNCATED)
                {
                    Marshal.FreeHGlobal(stringMem);
                    stringLength *= 2;
                    stringMem = Marshal.AllocHGlobal(stringLength);
                    result = FMOD_Studio_CommandReplay_GetCommandString(this.handle, commandIndex, stringMem, stringLength);
                }

                if (result == RESULT.OK)
                {
                    buffer = encoder.stringFromNative(stringMem);
                }
                Marshal.FreeHGlobal(stringMem);
                return result;
            }
        }
        public RESULT getCommandAtTime(float time, out int commandIndex)
        {
            return FMOD_Studio_CommandReplay_GetCommandAtTime(this.handle, time, out commandIndex);
        }
        // Playback
        public RESULT setBankPath(string bankPath)
        {
            using (StringHelper.ThreadSafeEncoding encoder = StringHelper.GetFreeHelper())
            {
                return FMOD_Studio_CommandReplay_SetBankPath(this.handle, encoder.byteFromStringUTF8(bankPath));
            }
        }
        public RESULT start()
        {
            return FMOD_Studio_CommandReplay_Start(this.handle);
        }
        public RESULT stop()
        {
            return FMOD_Studio_CommandReplay_Stop(this.handle);
        }
        public RESULT seekToTime(float time)
        {
            return FMOD_Studio_CommandReplay_SeekToTime(this.handle, time);
        }
        public RESULT seekToCommand(int commandIndex)
        {
            return FMOD_Studio_CommandReplay_SeekToCommand(this.handle, commandIndex);
        }
        public RESULT getPaused(out bool paused)
        {
            return FMOD_Studio_CommandReplay_GetPaused(this.handle, out paused);
        }
        public RESULT setPaused(bool paused)
        {
            return FMOD_Studio_CommandReplay_SetPaused(this.handle, paused);
        }
        public RESULT getPlaybackState(out PLAYBACK_STATE state)
        {
            return FMOD_Studio_CommandReplay_GetPlaybackState(this.handle, out state);
        }
        public RESULT getCurrentCommand(out int commandIndex, out float currentTime)
        {
            return FMOD_Studio_CommandReplay_GetCurrentCommand(this.handle, out commandIndex, out currentTime);
        }
        // Release
        public RESULT release()
        {
            return FMOD_Studio_CommandReplay_Release(this.handle);
        }
        // Callbacks
        public RESULT setFrameCallback(COMMANDREPLAY_FRAME_CALLBACK callback)
        {
            return FMOD_Studio_CommandReplay_SetFrameCallback(this.handle, callback);
        }
        public RESULT setLoadBankCallback(COMMANDREPLAY_LOAD_BANK_CALLBACK callback)
        {
            return FMOD_Studio_CommandReplay_SetLoadBankCallback(this.handle, callback);
        }
        public RESULT setCreateInstanceCallback(COMMANDREPLAY_CREATE_INSTANCE_CALLBACK callback)
        {
            return FMOD_Studio_CommandReplay_SetCreateInstanceCallback(this.handle, callback);
        }
        public RESULT getUserData(out IntPtr userdata)
        {
            return FMOD_Studio_CommandReplay_GetUserData(this.handle, out userdata);
        }
        public RESULT setUserData(IntPtr userdata)
        {
            return FMOD_Studio_CommandReplay_SetUserData(this.handle, userdata);
        }

        #region importfunctions
        [DllImport(STUDIO_VERSION.dll)]
        private static extern bool FMOD_Studio_CommandReplay_IsValid                    (IntPtr replay);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_GetSystem                (IntPtr replay, out IntPtr system);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_GetLength                (IntPtr replay, out float length);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_GetCommandCount          (IntPtr replay, out int count);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_GetCommandInfo           (IntPtr replay, int commandindex, out COMMAND_INFO info);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_GetCommandString         (IntPtr replay, int commandIndex, IntPtr buffer, int length);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_GetCommandAtTime         (IntPtr replay, float time, out int commandIndex);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_SetBankPath              (IntPtr replay, byte[] bankPath);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_Start                    (IntPtr replay);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_Stop                     (IntPtr replay);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_SeekToTime               (IntPtr replay, float time);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_SeekToCommand            (IntPtr replay, int commandIndex);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_GetPaused                (IntPtr replay, out bool paused);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_SetPaused                (IntPtr replay, bool paused);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_GetPlaybackState         (IntPtr replay, out PLAYBACK_STATE state);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_GetCurrentCommand        (IntPtr replay, out int commandIndex, out float currentTime);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_Release                  (IntPtr replay);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_SetFrameCallback         (IntPtr replay, COMMANDREPLAY_FRAME_CALLBACK callback);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_SetLoadBankCallback      (IntPtr replay, COMMANDREPLAY_LOAD_BANK_CALLBACK callback);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_SetCreateInstanceCallback(IntPtr replay, COMMANDREPLAY_CREATE_INSTANCE_CALLBACK callback);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_GetUserData              (IntPtr replay, out IntPtr userdata);
        [DllImport(STUDIO_VERSION.dll)]
        private static extern RESULT FMOD_Studio_CommandReplay_SetUserData              (IntPtr replay, IntPtr userdata);
        #endregion

        #region wrapperinternal

        public IntPtr handle;

        public bool hasHandle()     { return this.handle != IntPtr.Zero; }
        public void clearHandle()   { this.handle = IntPtr.Zero; }

        public bool isValid()
        {
            return hasHandle() && FMOD_Studio_CommandReplay_IsValid(this.handle);
        }

        #endregion
    }
} // FMOD
