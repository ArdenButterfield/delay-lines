# Echo Location user manual

Echo Location is an experimental delay effect, designed to let you realize your wildest tape delay dreams. From filters to flangers, resonators to reverbs, it's all delay, and it's all here in Echo Location.

Echo Location structures delay as a web of delay lines between points. When you first open the plugin, you will notice two points, one pulsing green outwards, and one pulsing red inwards. These are the input and output points: audio from earlier on the effect chain will enter to the input point, and audio will leave through the output point to get to later steps on the effect chain.

The real processsing, however, happens in the lines between points. These lines are the tape delays: they move audio from one point to another over time. Depending on the parameters set, they may change the audio in some way, making it louder or softer, panning it, or filtering and distorting the sound.

## The Web View

`IMAGE GOES HERE`

Echo Locations's default view is the "web view," as shown above. The controls are as follows:

- **Add a line** by dragging from the outer edge of a point. If you end your dragging at an existing point, the line will go to that point, otherwise it will create a new point at the end.
- **Add a point** by clicking on the web view somewhere where there is not an existing point or line.
- **Remove a point or line** by right-clicking on it.
- **Move a point** by dragging from the middle of the point.
- **"Spring" a point** by dragging the point while holding `Shift`. This will move the point, but will cause it to spring back to where it started after you release the mouse.
- **Edit a line** by clicking on it, to open up the line editor.
- **Close the line editor** by clicking outside the line editor.
- **Move the viewport** by dragging.

### Stretching Lines

Dragging a point does more than just changing the way the web looks. As a line gets longer or shorter, it actually changes the length of the delay line. As a delay line changes in length, it will shift lower or higher in pitch, as the line has to speed up to get ahead, or slow down to get farther behind.

## The Grid View

To see all line editors displayed side-by-side, you can use the Web/Grid button to switch to the grid view.

## The Line Editor

The line editor lets you precisely control the length and timbre of a delay line.

### Bypass/Mute/Stagnate

These three checkboxes control the input and output of the line. When the line is **bypassed**, the input audio is immediately sent out the other end, with no delay at all. This can be helpful when organizing a web, or to be able to stretch one line without stretching neighboring lines.
When the line is **muted**, no audio will be output by the line, completely silencing it. When the line is **stagnated**, no audio enters the line. By default, this means that no audio comes out of the line, like when the line is muted. However, unlike when it is muted, controls that change the length of the line (modulation and dragging points) will be heard.

### Length

There are multiple units available for the delay length: 
- **milliseconds** 
- **samples**, based on the project sample rate
- **hertz**
- **pitch**, tuned to A440, 12 tone equal temperment
- **beat**, which is set using a numerator and denominator system. For instance, to set the delay length to a dotted quarter note, you can set the first number (the numerator) to 3 and the second number (the denominator) to 8, for a three eighth note delay. Or, to set the delay to eigth-note triplets, set the numerator to 1 and the denominator to 12. If the DAW does not provide a bpm, this will default to 120 bpm.
- **MIDI input**. This setting watches for MIDI notes from the selected device in the MIDI device menu at the top of the GUI. When the program receives a note-on message, it sets the delay length to that pitch, shifted by the value set here. This lets you play melodies or even chord progressions out of nothing but delay.

### Drive

This is the amplification applied to the incoming signal entering the delay line. At 0, the signal will be completely silenced; at 1, the volume will be unchanged by this step (though may be changed by other input steps).

### Pan
This is the panning applied to the incoming signal entering the delay line. -1 corresponds to panning hard left, and 1 for hard right. At 0, there will be no panning applied: that is, neither channel will be amplified or attenuated. Note that, even if Drive is set to 1, if the track is panned, the track it is panned towards will be raised in volume.

### Feedback

This is the amount of the outgoing signal that will be fed back into the delay line. Note that drive and pan do not affect the feedback: both of those effects are applied before the feedback loop, whereas the right column of effects happens inside the delay line.

### Mod

This controls the depth and rate (in hertz) of the sine-wave modulation of the delay line's length. This is useful for flanging and chorus effects


### Filter

This allows you to set a low-cut and high-cut filter, measured in hertz.

### Distortion

This section allows you to apply a variety of types of distortion: a soft clip, a hard clip, a sinusoidal wavefold, bitcrush, and "packet loss" in both mono and stereo: this effect erases random segments.

### Copy/Paste

These buttons copy and paste the state of the line editor. That way, you can set one line to have the exact same settings as another, or save you favorite line settings in a file for later.

## The Mapping Section

Because of the changing nature of adding and removing lines, this plugin does not directly give you parameters that can be automated in the DAW. This section lets you map number automatable parameters to the internal parameters of the points and lines in the web. To map a parameter, click on the "map" button for that parameter, then click on the parameter. You can then control that parameter from the DAW automation or from the knob in the mapping area. To unmap that knob, click the button again.

## The Top Bar

### Stretch Time

This slider sets the time it takes a delay line to get to a new length when it is dragged or a length slider is moved. At 0, it fades from one length to the other, so you will not hear any of the pitch-shifting sound when dragging.

### Mix

Standard mix knob. At 0%, none of the sound of the delay lines will be heard; at 100%, only the sound of the delay lines will be heard.
