# coherent-ui-virtual-touch

This is an implementation of a "virtual touch" interface for Coherent UI in-game browser views, intended for use in virtual reality applications.   The implementation is in C++ for Unreal Engine. It's mainly intended as a proof-of-concept, but as hand tracking technologies become more and more robust it could be a useful input mechanism for virtual reality apps. 

Let's go straight to the video - you can see the "virtual touch" interface in action here: 

https://www.youtube.com/watch?v=3xvFLZI7FYA

In the example, a Leap Motion controller was used for the hand input, but in theory another hand tracking technology could be used, as long as you can get the finger and hand positions in Unreal Engine world space coordinates.

The classes also implement a mechanism for using a raytrace as an input mechanism which is still very useful even if you don't have a hand tracker like Leap Motion.  You can see this raytrace input in action here:

https://www.youtube.com/watch?v=j2rq2uUfXyE


The idea is to translate hand gestures into mouse events in the Coherent UI view.  For example if the finger crosses the Z plane of the Coherent UI view, it is translated to a mouse down or mouse up event.  Some rudimentary swipe gestures are also supported.  The supported gestures are

* "screen tap" - translates to mouse down and up event in Coherent UI
* vertical scrolling - translates to mouse scroll in Coherent UI
* left/right quick swipe (see video) - translates to browser back() call in coherent UI (single direction swipe led to too many false positves)

## How to integrate into Unreal Engine C++ project

* The Coherent UI plugin is required and it is a commercial product , so please visit their website at http://coherent-labs.com/ue4/ to register for a free trial and to read the instructions

* The UISurfaceActor class can be used to createa Blueprint in Unreal Engine.  Basically, follow this tutorial here except using UISurfaceActor as the base class: https://www.youtube.com/watch?v=yB3kThf5Op8  

* If you want to use virtual touch instead of just raytrace, then you will also need a Leap Motion Controller. I have implemented a convenience "LeapInputReader" class for integrating Leap Motion into Unreal Engine in my other Leap-related project here: https://github.com/lmalave/leap-virtual-joystick


add to BeginPlay(): 

	    controller = new Leap::Controller();
	    controller->setPolicy(Leap::Controller::POLICY_OPTIMIZE_HMD);
	    LeapInput = new LeapInputReader(controller, this); // OPTIONAL: this is using the convenience classes from my other leap-virtual-joystick github project
		UISurfaceRaytraceHandler = new UISurfaceRaytraceInputHandler(this, FirstPersonCameraComponent);

add to Tick(): 

        LeapInput->UpdateHandLocations();
		ActionHandPalmLocation = LeapInput->GetRightPalmLocation_WorldSpace();
		ActionHandFingerLocation = LeapInput->GetRightFingerLocation_WorldSpace();
		AUISurfaceActor* SelectedUISurface = UISurfaceRaytraceHandler->GetSelectedUISurfaceActor();
		if (SelectedUISurface) {
			SelectedUISurface->HandleVirtualTouchInput(ActionHandPalmLocation, ActionHandFingerLocation);
		}

That's it!  The virtual touch implementation is intended to be simple and minimalist and yet very effective as an actually usable Leap-based input system.

The actual virtual touch implementation only involves 2 classes (UISurfaceActor and UISurfaceRaytraceInputHandler), which are described below.  

## C++ Class Design and Implementation

The implementation was intended to be a thin wrapper around a CoherentUI view that would translate hand gestures (or raytrace input) to Coherent UI View mouse events.  The implementation involves 2 classes

* UISurfaceActor - wrapper around Coherent UI view that handles events
* UISurfaceRaytraceInputHandler - raytrace that is used to select the UISurfaceActor that has the input focus

### UISurfaceActor class
 
The job of the UISurfaceActor class is simply to translate hand and finger positions and movement to Coherent UI mouse events or browser actions.  Note that for now only one pointer finger is actually needed (in my examples I use middle finger because that was most reliable with the Leap Motion controller).  


The intended use is for the UISurfaceActor class to be used as the base class for a Blueprint so that the standard procedure to add a Coherent UI in-game view can be used (see tutorial referenced above).

### UISurfaceRaytraceInputHandler class
 
The job of the UISurfaceRaytraceInputHandler class is to select which UISurfaceActor currently has the input focus.  So all a user/player needs to do is look at the UI Surface and the raytrace will detect a hit result and give the UI Surface the input focus.   

In addition, the class actually draws the line of the raytrace to give clear visual feedback, and also passes the hit location as a mouseover event to the UI Surface.   

## Roadmap

* Better visual representation for raytrace than using DrawDebugLine (something like a translucent glowing ray)
* More robust vertical scrolling gesture (right now it's a bit slow)
* Additional gestures?  
* Porting same idea to Unity? 
