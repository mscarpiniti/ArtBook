#ifndef MIDI_EVENT_HANDLER_H
#define MIDI_EVENT_HANDLER_H

#include <portmidi.h>
#include <pmutil.h>
#include <string>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <chrono>


enum Events {
    NOTE_ON     = 0x9,
    NOTE_OFF    = 0x8,
    PITCHBEND   = 0xE,
    CTRL        = 0xB
};

class midi_events_handler
{
private:
    std::vector<const PmDeviceInfo*>* m_PmDevices;
    std::vector<std::string> *m_devices;
    PortMidiStream* m_theStream;
    PmQueue* m_midiEventQueue;
    bool m_midiMustRun;
    std::string m_currentDevName;

    static void ProcessMidi(int32_t, void*);
    void EnumerateDevices();
    void MidiMsgLoop();

public:
    midi_events_handler();
    virtual ~midi_events_handler();

    void Init();

    std::vector<std::string>& GetDevicesVector();
    void SetMidiDevice(std::string);

    inline PortMidiStream* GetMidiStreamHandle() {
        return m_theStream;
    }

    inline std::string GetMidiDevice() {
        return m_currentDevName;
    }

    inline PmQueue* GetMidiEventList() {
        return m_midiEventQueue;
    }
};

#endif // MIDI_EVENT_HANDLER_H
