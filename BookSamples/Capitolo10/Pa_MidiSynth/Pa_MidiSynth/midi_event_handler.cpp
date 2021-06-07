#include "midi_event_handler.h"
#include <porttime.h>

void midi_events_handler::ProcessMidi(int32_t p_tstamp, void *p_udata) {
    PmError l_result;
    PmEvent l_buffer; /* just one message at a time */
    (void)p_tstamp;
    midi_events_handler* l_data = static_cast<midi_events_handler*>(p_udata);

    /* see if there is any midi input to process */
    do {
        l_result = Pm_Poll(l_data->m_theStream);
        if (l_result) {
            if (Pm_Read(l_data->m_theStream, &l_buffer, 1) == pmBufferOverflow)
                continue;
            Pm_Enqueue(l_data->m_midiEventQueue, &l_buffer.message);
        }
    } while (l_result);
}

void midi_events_handler::EnumerateDevices()
{
    int l_count = Pm_CountDevices();
    const PmDeviceInfo *l_info;

    for (int j = 0; j < l_count; j++) {
        l_info = Pm_GetDeviceInfo(j);
        if(l_info->input) {
            m_devices->push_back(l_info->name);
            m_PmDevices->push_back(l_info);
        }
    }
}

midi_events_handler::midi_events_handler()
{
    m_devices = new std::vector<std::string>();
    m_PmDevices = new std::vector<const PmDeviceInfo*>();
    m_midiEventQueue = Pm_QueueCreate(16, sizeof(int32_t));
}

midi_events_handler::~midi_events_handler()
{
    Pt_Stop();
    Pm_Close(m_theStream);
    Pm_Terminate();
    Pm_QueueDestroy(m_midiEventQueue);
}

void midi_events_handler::Init()
{
    PmError l_err = pmNoError;
    Pm_Initialize();
    EnumerateDevices();
    m_currentDevName = "";
    int l_defDev = -1;

    //set the first device found as default
    for (size_t k = 0; k < m_PmDevices->size(); k++) {
        if ((*m_PmDevices)[k] != nullptr) {
            m_currentDevName = (*m_PmDevices)[k]->name;
            l_defDev = k;
            break;
        }
    }

    if (l_defDev >= 0) {
        l_err = Pm_OpenInput(&m_theStream,(*m_PmDevices)[l_defDev]->input,nullptr,0,nullptr,nullptr);
        if (l_err == pmNoError) {
            Pt_Start(1, &ProcessMidi, this);
        }
    }
}

std::vector<std::string> &midi_events_handler::GetDevicesVector()
{
    if (m_devices == nullptr) {
        EnumerateDevices();
    }
    return *m_devices;
}

void midi_events_handler::SetMidiDevice(std::string p_dev)
{
    if (p_dev != "") {
        Pt_Stop();
        Pm_Close(&m_theStream);
        Pm_Terminate();
    }

    if (p_dev != m_currentDevName) {
        for (size_t k = 0; k < m_PmDevices->size(); k++) {
            const PmDeviceInfo* info = (*m_PmDevices)[k];
            if (info->name == p_dev) {
                Pm_Initialize();
                PmError l_err = Pm_OpenInput(&m_theStream,k,nullptr,0,nullptr,nullptr);
                if(pmNoError == l_err) {
                    m_currentDevName = info->name;
                    Pt_Start(1, &ProcessMidi, this);
                }
                break;
            }
        }
    }
}
