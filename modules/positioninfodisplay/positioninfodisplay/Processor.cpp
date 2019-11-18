/*
 Author: Martin Finke
 Date:   Nov 18, 2019 09:00
 */

Processor::Processor()
#ifndef JucePlugin_PreferredChannelConfigurations
: juce::AudioProcessor(BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
                           .withInput("Input", AudioChannelSet::stereo(), true)
#endif
                           .withOutput("Output", AudioChannelSet::stereo(), true)
#endif
)
#endif
{
    latestPositionInfoOnAudioThread.resetToDefault();
    startTimer(/* intervalInMilliseconds: */ 16);
}

void Processor::addListener(PositionInfoListener& listener)
{
    positionInfoListeners.add(&listener);
}

void Processor::removeListener(PositionInfoListener& listener)
{
    positionInfoListeners.remove(&listener);
}

const String Processor::getName() const
{
    return JucePlugin_Name;
}

bool Processor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Processor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Processor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double Processor::getTailLengthSeconds() const
{
    return 0.0;
}

int Processor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are 0 programs,
        // so this should be at least 1, even if you're not really implementing programs.
}

int Processor::getCurrentProgram()
{
    return 0;
}

void Processor::setCurrentProgram(int)
{
}

const String Processor::getProgramName(int)
{
    return {};
}

void Processor::changeProgramName(int, const String&)
{
}

void Processor::prepareToPlay(double, int)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void Processor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Processor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void Processor::processBlock(AudioBuffer<float>&, MidiBuffer&)
{
    if (auto* playHead = getPlayHead()) {
        AudioPlayHead::CurrentPositionInfo newPositionInfo;
        if (playHead->getCurrentPosition(newPositionInfo)) {
            if (newPositionInfo != latestPositionInfoOnAudioThread) {
                latestPositionInfoOnAudioThread = newPositionInfo;
                positionInfoQueue.enqueue({ latestPositionInfoOnAudioThread, Time::getCurrentTime() });
            }
        }
        else {
            latestPositionInfoOnAudioThread.resetToDefault();
            positionInfoQueue.enqueue({ latestPositionInfoOnAudioThread, Time::getCurrentTime() });
        }
    }
}

bool Processor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* Processor::createEditor()
{
    return new Editor(*this);
}

void Processor::getStateInformation(MemoryBlock&)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Processor::setStateInformation(const void*, int)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

void Processor::timerCallback()
{
    JUCE_ASSERT_MESSAGE_THREAD;

    pair<AudioPlayHead::CurrentPositionInfo, Time> newPositionInfo;
    bool positionInfoChanged = false;
    while (positionInfoQueue.try_dequeue(newPositionInfo))
        positionInfoChanged = true;
    
    if (positionInfoChanged)
        positionInfoListeners.call([&newPositionInfo](auto& listener) { listener.positionInfoChanged(newPositionInfo.first, newPositionInfo.second); });
}
