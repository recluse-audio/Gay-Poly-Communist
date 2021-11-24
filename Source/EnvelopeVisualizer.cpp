/*
  ==============================================================================

    EnvelopeVisualizer.cpp
    Created: 7 Nov 2020 8:53:59am
    Author:  Billie (Govnah) Jean

  ==============================================================================
*/

#include "EnvelopeVisualizer.h"

//==============================================================================
EnvelopeVisualizer::EnvelopeVisualizer(GayPolyCommunistAudioProcessor& p) : audioProcessor(p)
{
    setSize(400, 150);

    putValuesInTable();

    pointSize = getWidth() * 0.01f;

    reducedFrame.setBounds(getWidth() * 0.1f, getHeight() * 0.1f, getWidth() * 0.8f, getHeight() * 0.98f);
    
    auto width = reducedFrame.getWidth();
    auto height = reducedFrame.getHeight() / 2.f;

    startPoint.setCentre(reducedFrame.getX(), height);
    startPoint.setSize(pointSize, pointSize);

    peakPoint.setCentre(reducedFrame.getX() + width * 0.2f, reducedFrame.getY());
    peakPoint.setSize(pointSize, pointSize);

    sustainPoint.setCentre(reducedFrame.getX() + width * 0.35f, reducedFrame.getY() + height * 0.2f);
    sustainPoint.setSize(pointSize, pointSize);

    releasePoint.setCentre(reducedFrame.getX() + width * 0.5f, reducedFrame.getY() + height * 0.2f);
    releasePoint.setSize(pointSize, pointSize);
    
    endPoint.setCentre(reducedFrame.getWidth() + width * 0.7f, height);
    endPoint.setSize(pointSize, pointSize);



    putValuesInTable();

    startTimerHz(60);
}

EnvelopeVisualizer::~EnvelopeVisualizer()
{
}

/*
    what owns the graphic context of the component class?  What calls the paint method at all?
    looks like ComponentPeer is a class that does the real nitty gritty work involved in making
    a component, which is really just a graphics content if you think about it.
*/
void EnvelopeVisualizer::paint (juce::Graphics& g)
{
    envPath.clear();
    sliderPath.clear();

    // background
    g.setColour(juce::Colours::black);
    g.fillAll();
    
    // Outer Frame
    g.setColour(juce::Colours::orange);
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 1.f, 1.f);

    // Inner Fram
    g.setColour(juce::Colours::orange.withAlpha(0.3f));
    g.drawRoundedRectangle(reducedFrame.toFloat(), 1.f, 1.f);
    
    // Env points
    g.setColour(juce::Colours::skyblue.withLightness(0.6f));
    g.drawEllipse(startPoint, pointSize);
    g.drawEllipse(peakPoint, pointSize);
    g.drawEllipse(sustainPoint, pointSize);
    g.drawEllipse(releasePoint, pointSize);
    g.drawEllipse(endPoint, pointSize);

    envPath.startNewSubPath(startPoint.getCentre());
    envPath.lineTo(peakPoint.getCentre());
    envPath.lineTo(sustainPoint.getCentre());
    envPath.lineTo(releasePoint.getCentre());
    
    //sortEnvPoints();

   /* for (int i = 1; i < envPoints.size(); ++i)
    {
        auto* point = envPoints[i];
        g.drawEllipse(*point, pointSize * 0.3f);
        envPath.lineTo(point->getCentre());
    }*/

    // draw env

    envPath.lineTo(endPoint.getCentre());


    juce::PathStrokeType stroke(0.75f, juce::PathStrokeType::curved);
    g.strokePath(envPath, stroke);
    
    envChange = false;
    
    // middle line to 
    sliderPath.startNewSubPath(frame.getX(), frame.getHeight() * 0.5f);
    sliderPath.lineTo(getWidth(), frame.getHeight() * 0.5f);
    juce::PathStrokeType sliderStroke(1.75f, juce::PathStrokeType::curved);
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    g.strokePath(sliderPath, sliderStroke);

}

void EnvelopeVisualizer::sortEnvPoints()
{
    auto sortLambda = [](EnvelopeControlPoint<float>* x1, EnvelopeControlPoint<float>* x2)
    {
        return x1->getX() < x2->getX();
    };
    
    std::sort(envPoints.begin(), envPoints.end(), sortLambda);
    
}

bool EnvelopeVisualizer::compareX(EnvelopeControlPoint<float> x1, EnvelopeControlPoint<float> x2)
{
    return x1.getX() < x2.getX();
}

void EnvelopeVisualizer::resized()
{
    reducedFrame.setBounds(getWidth() * 0.1f, getHeight() * 0.1f, getWidth() * 0.8f, getHeight() * 0.7f);
    pointSize = getWidth() * 0.01f;

    auto width = reducedFrame.getWidth();
    auto height = reducedFrame.getHeight();

    startPoint.setCentre(reducedFrame.getX(), reducedFrame.getY() + height);
    startPoint.setSize(pointSize * 0.2f, pointSize * 0.2f);

    peakPoint.setCentre(reducedFrame.getX() + width * 0.2f, reducedFrame.getY() + height * 0.02f);
    peakPoint.setSize(pointSize, pointSize);

    sustainPoint.setCentre(reducedFrame.getX() + width * 0.35f, reducedFrame.getY() + height * 0.2f);
    sustainPoint.setSize(pointSize, pointSize);

    releasePoint.setCentre(reducedFrame.getX() + width * 0.5f, reducedFrame.getY() + height * 0.2f);
    releasePoint.setSize(pointSize, pointSize);

    endPoint.setCentre(reducedFrame.getX() + width, reducedFrame.getY() + height);
    endPoint.setSize(pointSize * 0.2f, pointSize * 0.2f);

}



void EnvelopeVisualizer::setName(juce::String mName)
{
    //nameLabel->setText(mName, dontSendNotification);
    repaint();
}

void EnvelopeVisualizer::mouseDown(const MouseEvent& e)
{
    // change flag
    envChange = true;

    mousePosition.setXY(e.x, e.y);
    lastPosition.setXY(e.x, e.y);
        
    // check if our cursor is inside one the dots in the vector
    // first the terminal points
    if (startPoint.expanded(5.f).contains(mousePosition))
    {
        isDraggingStartPoint = true;
    }

    if (peakPoint.expanded(5.f).contains(mousePosition))
    {
        isDraggingPoint = true;
        selectedPoint = &peakPoint;
    }

    if (sustainPoint.expanded(5.f).contains(mousePosition))
    {
        isDraggingPoint = true;
        selectedPoint = &sustainPoint;
    }

    if (releasePoint.expanded(5.f).contains(mousePosition))
    {
        isDraggingPoint = true;
        selectedPoint = &releasePoint;

    }
    
    if (endPoint.expanded(5.f).contains(mousePosition))
    {
        isDraggingEndPoint = true;
    }
    
    // look through the array if we aren't grabbing the start or end points
    if (!isDraggingStartPoint || !isDraggingEndPoint)
    {
        for (size_t i = 0; i < envPoints.size(); ++i)
        {
            auto point = envPoints[i];
            if(point->expanded(5.f).contains(mousePosition))
            {
                // delete point
                if(e.mods.isCommandDown() || e.mods.isRightButtonDown())
                {
                    envPoints.removeObject(point);
                }
                else // draggin point
                {
                    isDraggingPoint = true;
                    selectedPoint = point;
                }
                    
            }
        }
    }

}

void EnvelopeVisualizer::mouseUp(const MouseEvent& e)
{
    isDraggingPoint = false;
    isDraggingStartPoint = false;
    isDraggingEndPoint = false;
    //env.resetIndex();
    putValuesInTable();
}

void EnvelopeVisualizer::mouseMove(const MouseEvent& e)
{
    
}

void EnvelopeVisualizer::mouseDoubleClick(const MouseEvent& e)
{
    //mousePosition.setXY(e.x, e.y);
    //auto* newPoint = envPoints.add(new EnvelopeControlPoint<float>);
    //newPoint->setPosition(e.x, e.y);
    //newPoint->setSize(pointSize, pointSize);
    //
}

void EnvelopeVisualizer::mouseDrag(const MouseEvent& e)
{
    envChange = true;

    auto x = jlimit(reducedFrame.getX(), reducedFrame.getX() + reducedFrame.getWidth(), e.x);
    auto y = jlimit(reducedFrame.getY(), reducedFrame.getY() + reducedFrame.getHeight(), e.y);

    mousePosition.setXY(e.x, y);
        
    if (isDraggingStartPoint)
    {
        startPoint.setCentre(reducedFrame.getX(), y);
    }
    else if (isDraggingEndPoint)
    {
        endPoint.setCentre(reducedFrame.getX() + reducedFrame.getWidth(), y);
    }
    else if (isDraggingPoint)
    {
        selectedPoint->setCentre(e.x, y);
            
        // check to make sure no dots have the same x value
        //for(size_t i = 0; i < envPoints.size(); ++i)
        //{
        //    auto* point = envPoints[i]; // point we are checking against
        //    if (point->containsWithinSection(selectedPoint->getCentre()) && !point->isSelected())
        //    {
        //        selectedPoint->translate(5.f, 0.f);
        //    }
        //}
    } 
}

/*
    TO DO: set time of sections of adsr as well as peak and sustain level.
    Attach this to the value tree
*/
void passADSR()
{
    audioProcessor.
}

void EnvelopeVisualizer::putValuesInTable()
{
    sortEnvPoints();
    envPoints.insert(0, &startPoint); //  add start / end point to the array for this calculation only
    envPoints.add(&endPoint);
    
    int xDelt = 0;
    for (size_t i = 1; i < envPoints.size(); ++i)
    {
        auto height = reducedFrame.getHeight();
        auto y1 = height - envPoints[i-1]->getCentreY();
        auto y2 = height - envPoints[i]->getCentreY();
        float val1 = y1 / height;
        float val2 = y2 / height;
        float yDelta = val2 - val1; // change from previous point to current point in y axis scaled 0 - 1
        
        auto width = reducedFrame.getWidth();
        auto x1 = envPoints[i-1]->getCentre().getX();
        
        auto x2 = envPoints[i]->getCentre().getX();
        
        
        auto envelopeWidth = endPoint.getCentre().getX() - startPoint.getCentre().getX();
        float xDeltaPercent = (x2 - x1) / envelopeWidth; // percent of x axis this section is
        
        float xDeltaSamples = xDeltaPercent * 1024; // same percent applied to the 48000 samples in buffer
        float yDeltaPerSample = yDelta / xDeltaSamples; // how much change in amp 0 - 1 over the section
        
        xDelt += (int) xDeltaSamples;
        
        
        for (int j = 0; j <= (int) xDeltaSamples; ++j)
        {
            float yValue = val1 + (yDeltaPerSample * j);
            
            if(y1 < 0.01)
                yValue = 0;
            if(yValue > 0.99)
                yValue = 1;
            
            //env.setValueAtCurrentIndex(yValue);
        }
        // interpolate from y1 to y2 over xDeltaSamples
    }
    //int hangoverSamples = env.getTable().getNumSamples() - xDelt;
    //
    //for (int i = 0; i < hangoverSamples; ++i)
    //{
    //   env.setValueAtCurrentIndex(0.f);
    //}
    envPoints.remove(0, false); // remove start point
    envPoints.remove(envPoints.size()-1, false); 
    
}

void EnvelopeVisualizer::timerCallback()
{
    repaint();
}


void EnvelopeVisualizer::sliderValueChanged(Slider* s)
{
    // % change in slider reflected as % change in points
    auto currentSliderVal = s->getValue();
    
    
    
    auto height = frame.getHeight();
    
    auto change = (currentSliderVal - lastSliderVal) * height;
    
    auto y1 = startPoint.getBottom() - change;


    if (y1 > height - 5)
        y1 = height - 5;
    if (y1 < 5)
        y1 = 5;

    startPoint.setY(y1);

    auto y2 = endPoint.getBottom() - change;


    if (y2 > height - 5)
        y2 = height - 5;
    if (y2 < 5)
        y2 = 5;

    endPoint.setY(y2);
    
    for (size_t i = 0; i < envPoints.size(); ++i)
    {
        auto y3 = envPoints[i]->getY() - change;
        //y3 = envPoints[i]->getY() - y3;
        
        if (y3 > height - 5)
            y3 = height - 5;
        
        if (y3 < 5)
            y3 = 5;
        
        envPoints[i]->setY(y3);

    }
    
    repaint();
    putValuesInTable();
    
    lastSliderVal = s->getValue();
}
