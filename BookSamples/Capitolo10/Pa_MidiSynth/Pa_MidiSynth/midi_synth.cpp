#include "midi_synth.h"
//#include <porttime.h>
#include <math.h>

static int Synth_callback( const void *p_inputBuffer, void *p_outputBuffer,
                            unsigned long p_framesPerBuffer,
                            const PaStreamCallbackTimeInfo* p_timeInfo,
                            PaStreamCallbackFlags p_statusFlags,
                            void *p_userData )
{
    synth_parameters *l_data = (synth_parameters*)p_userData;
    float *l_out = (float*)p_outputBuffer;
    unsigned long i;
    (void) p_timeInfo; /* Prevent unused variable warnings. */
    (void) p_statusFlags;
    (void) p_inputBuffer;

    int32_t l_msg;
    uint8_t l_status, l_data1, l_data2;
    uint16_t l_pitch = 0;
    unsigned char l_event;
    PmQueue* l_localQueue = l_data->midiEvents;

    // Dequeue midi events and process them
    for( i=0; i < p_framesPerBuffer; i++ )
    {
        while (!Pm_QueueEmpty(l_localQueue)) {
            Pm_Dequeue(l_localQueue, &l_msg);
            l_status = Pm_MessageStatus(l_msg);
            l_data1 = Pm_MessageData1(l_msg);
            l_data2 = Pm_MessageData2(l_msg);

            l_event = (l_status & 0xF0) >> 4;

            switch (l_event) {
            using namespace std;
            case NOTE_ON:
                l_data->voices->PlayVoice(l_data1,l_data2);
                break;
            case NOTE_OFF:
                l_data->voices->MuteVoice(l_data1);
                break;
            case PITCHBEND:            
                l_pitch = (l_data2 << 7) + l_data1;
                l_data->voices->SetParameter(Pitch,((float)(l_pitch - 8192)/8191),0);
                break;

            case CTRL:
                switch (l_data1) {
                case 1:
                    l_data->voices->SetParameter(ModulationUp,0.0,l_data2);
                    break;
                case 2:
                    l_data->voices->SetParameter(ModulationDown,0.0,l_data2);
                    break;
                case 64:
                    l_data->voices->SetParameter(Sustain,0.0,l_data2);
                }
                break;
            }
        }

        *l_out++ = l_data->voices->NewLeftSample();
        *l_out++ = l_data->voices->NewRightSample();
    }

    return paContinue;
}

midi_synth::midi_synth(QObject *p_parent) : QObject(p_parent)
{
    m_midi_handler = new midi_events_handler();
    m_params = new synth_parameters();
    m_params->midiEvents = m_midi_handler->GetMidiEventList();
    m_params->voices = new VoiceBank();
}

midi_synth::~midi_synth()
{
    delete m_midi_handler;
    Pa_StopStream(&m_aStream);
    Pa_CloseStream(&m_aStream);
    Pa_Terminate();
}

void midi_synth::Init()
{
    PaError l_err = paNoError;
    m_midi_handler->Init();

    //Portaudio initialization, the same in all book's projects
    l_err = Pa_Initialize();
    m_outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    m_outputParameters.channelCount = 2;       /* stereo output */
    m_outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    m_outputParameters.suggestedLatency = Pa_GetDeviceInfo(m_outputParameters.device )->defaultLowOutputLatency;
    m_outputParameters.hostApiSpecificStreamInfo = NULL;
    l_err = Pa_OpenStream(&m_aStream,NULL,&m_outputParameters,44100,64,paClipOff,Synth_callback,m_params);
    l_err = Pa_StartStream(m_aStream);

    if(l_err != paNoError) { //stop processing and deallocate resource;
        Pa_StopStream(m_aStream);
    }
}

std::vector<std::string> &midi_synth::GetMidiDeviceList()
{
    return m_midi_handler->GetDevicesVector();
}

std::string midi_synth::GetCurrentMidiDevice()
{
    return m_midi_handler->GetMidiDevice();
}

void midi_synth::Cutoff(double p_val)
{
    m_params->voices->SetParameter(LpfCutoff,p_val,0);
}

void midi_synth::Osc1_Wave(QString wv)
{
    WaveForms wave;

    if (wv == "Sine") wave = Sine;
    if (wv == "SawTooth") wave = SawTooth;
    if (wv == "Square") wave = Square;

    m_params->voices->SetParameter(Osc1_wave,0.0,(int)wave);
}

void midi_synth::Osc1_Level(double p_val)
{
    m_params->voices->SetParameter(Osc1_level,p_val,0);
}

void midi_synth::Osc2_Wave(QString p_wv)
{
    WaveForms l_wave;

    if (p_wv == "Sine") l_wave = Sine;
    if (p_wv == "SawTooth") l_wave = SawTooth;
    if (p_wv == "Square") l_wave = Square;

    m_params->voices->SetParameter(Osc2_wave,0.0,(int)l_wave);
}

void midi_synth::Osc2_Level(double p_val)
{
    m_params->voices->SetParameter(Osc2_level,p_val,0);
}

void midi_synth::Osc2_Detune(double p_val)
{
    m_params->voices->SetParameter(Osc2_detune,p_val,0);
}

void midi_synth::Noise_Level(double p_val)
{
    m_params->voices->SetParameter(NoiseLevel,p_val,0);
}

void midi_synth::Attack_time(double p_val) {
    m_params->voices->SetParameter(Atk_time,p_val,0);
}

void midi_synth::Decay_time(double p_val) {
    m_params->voices->SetParameter(Dcy_time,p_val,0);
}

void midi_synth::Decay_level(double p_val) {
    m_params->voices->SetParameter(Dcy_lev,p_val/1000.0,0);
}

void midi_synth::Sus_time(double p_val) {
    m_params->voices->SetParameter(Sust_time,p_val,0);
}

void midi_synth::Sus_level(double p_val) {
    m_params->voices->SetParameter(Sust_lev,p_val/1000.0,0);
}

void midi_synth::Release_Time(double p_val) {
    m_params->voices->SetParameter(Rel_time,p_val,0);
}

void midi_synth::SetMidiDevice(std::string p_dev)
{
    m_midi_handler->SetMidiDevice(p_dev);
}
