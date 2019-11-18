/*
 Author: Martin Finke
 Date:   Nov 18, 2019 09:00
 */

class Processor : public juce::AudioProcessor, private Timer
{
public:
    class PositionInfoListener
    {
    public:
        virtual ~PositionInfoListener() = default;

        virtual void positionInfoChanged(const AudioPlayHead::CurrentPositionInfo&, Time) = 0;
    };

    Processor();
    
    void addListener(PositionInfoListener& listener);
    void removeListener(PositionInfoListener& listener);

    void prepareToPlay(double, int) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout&) const override;
#endif

    void processBlock(AudioBuffer<float>&, MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int) override;
    const String getProgramName(int) override;
    void changeProgramName(int, const String&) override;
    void getStateInformation(MemoryBlock&) override;
    void setStateInformation(const void*, int) override;

private:
    AudioPlayHead::CurrentPositionInfo latestPositionInfoOnAudioThread;
    ReaderWriterQueue<pair<AudioPlayHead::CurrentPositionInfo, Time>> positionInfoQueue;
    ListenerList<PositionInfoListener> positionInfoListeners;

    void timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Processor)
};
