/*
 Author: Martin Finke
 Date:   Nov 18, 2019 09:00
 */

class Editor : public juce::AudioProcessorEditor, private Processor::PositionInfoListener
{
public:
    Editor(Processor&);
    ~Editor() override;

    void paint(Graphics&) override; /** @internal */
    void resized() override; /** @internal */

private:
    Processor& processor;
    AudioPlayHead::CurrentPositionInfo positionInfo;
    StringArray textLines;
    TextEditor positionInfoDisplay;
    ComponentBoundsConstrainer boundsConstrainer;
    ResizableCornerComponent resizableCorner{ this, &boundsConstrainer };
    
    void updateText();
    void positionInfoChanged(const AudioPlayHead::CurrentPositionInfo&, Time) override;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor)
};
