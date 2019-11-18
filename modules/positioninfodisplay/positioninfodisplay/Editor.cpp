/*
 Author: Martin Finke
 Date:   Nov 18, 2019 09:00
 */

namespace {
const String indent = "    ";
const String true_("true");
const String false_("false");

forcedinline const String& boolToString(bool b)
{
    return (b ? true_ : false_);
}
}


Editor::Editor(Processor& p)
: juce::AudioProcessorEditor(&p),
  processor(p)
{
    // The CurrentPositionInfo struct has 14 members, plus one line for the title
    positionInfo.resetToDefault();
    textLines.ensureStorageAllocated(14 + 1);

    boundsConstrainer.setSizeLimits(/* minimumWidth: */ 100, /* minimumHeight: */ 50, /* maximumWidth: */ INT_MAX, /* maximumHeight: */ INT_MAX);

    setSize(600, 400);

    positionInfoDisplay.setMultiLine(/* shouldBeMultiLine: */ true, /* shouldWordWrap: */ true);
    positionInfoDisplay.setEscapeAndReturnKeysConsumed(false);
    positionInfoDisplay.setReadOnly(true);
    positionInfoDisplay.setText(TRANS("No CurrentPositionInfo received yet"), /* sendTextChangeMessage: */ true);

    addAndMakeVisible(positionInfoDisplay);
    addAndMakeVisible(resizableCorner);

    processor.addListener(*this);
}

Editor::~Editor()
{
    processor.removeListener(*this);
}

void Editor::paint(Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(Colours::black);
}

void Editor::resized()
{
    positionInfoDisplay.setBounds(getLocalBounds());

    resizableCorner.setBounds(getLocalBounds().removeFromRight(16).removeFromBottom(16));
}

void Editor::updateText()
{
    textLines.clear();

    textLines.add("AudioPlayHead::CurrentPositionInfo from " + Time::getCurrentTime().toISO8601(/* includeDividerCharacters: */ true) + " (milliseconds hi-res: " + String(Time::getMillisecondCounterHiRes(), 4) + "):");

    textLines.add(indent + "bpm: " + String(positionInfo.bpm));
    textLines.add(indent + "timeSigNumerator: " + String(positionInfo.timeSigNumerator));
    textLines.add(indent + "timeSigDenominator: " + String(positionInfo.timeSigDenominator));
    textLines.add(indent + "timeInSamples: " + String(positionInfo.timeInSamples));
    textLines.add(indent + "timeInSeconds: " + String(positionInfo.timeInSeconds));
    textLines.add(indent + "editOriginTime: " + String(positionInfo.editOriginTime));
    textLines.add(indent + "ppqPosition: " + String(positionInfo.ppqPosition));
    textLines.add(indent + "ppqPositionOfLastBarStart: " + String(positionInfo.ppqPositionOfLastBarStart));
    textLines.add(indent + "ppqLoopStart: " + String(positionInfo.ppqLoopStart));
    textLines.add(indent + "ppqLoopEnd: " + String(positionInfo.ppqLoopEnd));
    textLines.add(indent + "isPlaying: " + boolToString(positionInfo.isPlaying));
    textLines.add(indent + "isRecording: " + boolToString(positionInfo.isRecording));
    textLines.add(indent + "isLooping: " + boolToString(positionInfo.isLooping));

    positionInfoDisplay.setText(textLines.joinIntoString("\n"));
}

void Editor::positionInfoChanged(const AudioPlayHead::CurrentPositionInfo& newPositionInfo, Time)
{
    JUCE_ASSERT_MESSAGE_THREAD;

    if (newPositionInfo != positionInfo) {
        positionInfo = move(newPositionInfo);
        updateText();
    }
}
