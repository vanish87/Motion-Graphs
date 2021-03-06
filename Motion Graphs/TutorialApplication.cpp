/*
-----------------------------------------------------------------------------
Filename:    TutorialApplication.cpp
-----------------------------------------------------------------------------

This source file is part of the
   ___                 __    __ _ _    _ 
  /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
/ \_// (_| | | |  __/  \  /\  /| |   <| |
\___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
      |___/                              
      Tutorial Framework
      http://www.ogre3d.org/tikiwiki/
-----------------------------------------------------------------------------
*/
//#include "DotScene.h"
#include "stdafx.h"
#include "TutorialApplication.h"

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
	state = SET_USER_PATH;
}
//-------------------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
}

//-------------------------------------------------------------------------------------

void TutorialApplication::createCamera(void) {
    // create the camera
    mCamera = mSceneMgr->createCamera("PlayerCam");
    // set its position, direction  
    mCamera->setPosition(Ogre::Vector3(0, 25, 50));
    mCamera->lookAt(Ogre::Vector3(0, 0, 0));
    // set the near clip distance
    mCamera->setNearClipDistance(5);

    mCameraMan = new OgreBites::SdkCameraMan(mCamera); // create a default camera controller
}
//-------------------------------------------------------------------------------------

void TutorialApplication::createViewports(void) {
    // Create one viewport, entire window
    Ogre::Viewport* vp = mWindow->addViewport(mCamera);
    vp->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
    // Alter the camera aspect ratio to match the viewport
    mCamera->setAspectRatio(Ogre::Real(vp->getActualWidth()) / Ogre::Real(vp->getActualHeight()));
}
//-------------------------------------------------------------------------------------
bool TutorialApplication::mousePressed(const OIS::MouseEvent &arg, OIS::MouseButtonID id) {
    // left mouse button pressed
    if (id == OIS::MB_Left) {

        // get window height and width
        Ogre::Real screenWidth = Ogre::Root::getSingleton().getAutoCreatedWindow()->getWidth();
        Ogre::Real screenHeight = Ogre::Root::getSingleton().getAutoCreatedWindow()->getHeight();

        // convert to 0-1 offset
        Ogre::Real offsetX = arg.state.X.abs / screenWidth;
        Ogre::Real offsetY = arg.state.Y.abs / screenHeight;

        // set up the ray
        Ogre::Ray mouseRay = mCamera->getCameraToViewportRay(offsetX, offsetY);

        // check if the ray intersects our plane
        // intersects() will return whether it intersects or not (the bool value) and
        // what distance (the Real value) along the ray the intersection is
        std::pair<bool, Ogre::Real> result = mouseRay.intersects(*mPlane);

        if (result.first) {
            // if the ray intersect the plane, we have a distance value
            // telling us how far from the ray origin the intersection occurred.
            // the last thing we need is the point of the intersection.
            // Ray provides us getPoint() function which returns a point
            // along the ray, supplying it with a distance value.

            // get the point where the intersection is
            Ogre::Vector3 point = mouseRay.getPoint(result.second);

            printf("Mouse pressed\n");
            printf("X: %f  Y: %f  Z: %f\n", point.x, point.y++, point.z);

            //spline->addPoint(point);
            mWalkList.push_back(point);
            //splines->update();
            lines->addPoint(point);
            lines->update();
        }

    }

    if (mTrayMgr->injectMouseDown(arg, id)) return true;
    mCameraMan->injectMouseDown(arg, id);

    return true;
}

void TutorialApplication::initPathSynthesis()
{
    // TODO: Aceder aqui ao grafo    
	// Get all animations and get an iterator
    AnimSSet = mEntity->getAllAnimationStates();
	assIte = new Ogre::AnimationStateIterator(AnimSSet->getAnimationStateIterator());

	// skip some animations just to speed up 
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();
	assIte->moveNext();assIte->moveNext();

	mAnimationState = assIte->current()->second;
    mAnimationState->setEnabled(true);
	mAnimationState->setLoop(false);
	mAnimationState->setTimePosition(0);

    // Clean old frame positions
    framePositionCollection.clear();

	// update what animation we are current doing
	animationStep = 1;
	currentAnimationError = 0xffffffff;

	// get the arc lenght of the user defined path
	arcLenghtLinesPath = (mWalkList[animationStep] - mWalkList[animationStep-1]).length();

	// initialize the animation that will be for the user defined path
	choosenAnimation.first = assIte->current()->second;
	choosenAnimation.second = 0xFFFFFFFF;
		
	// print on screen what we are doing
	Ogre::String str = "Current animation is ";
	str.append(mAnimationState->getAnimationName());
	mInfoLabel->setCaption(str);

}

bool TutorialApplication::keyPressed(const OIS::KeyEvent &arg) {

	// reset program
	if (arg.key == OIS::KC_RETURN)
	{
		state = SET_USER_PATH ;

		mWalkList.clear();
		lines->clear();
		lines_path_done->clear();

		mWalkList.push_back(Ogre::Vector3(0, 1, 0));
		lines->addPoint(Ogre::Vector3(0, 1, 0));
		lines->update();

		// initialize the animation Path, 
		animationPath = new std::vector< Ogre::String >;
	}
	
    if (arg.key == OIS::KC_SPACE) // toggle visibility of advanced frame stats
    {
		initPathSynthesis();

		state = CALC_AVATAR_PATH;
		anim_state = AVATAR_ANIM_IN_CALC;

		// initialize the animation Path, 
		animationPath = new std::vector< Ogre::String >;
		
		//set the node and entity to origin position
		mNode->setPosition(0,0,0);
		mEntity->setDisplaySkeleton(true);
		mEntity->getSkeleton()->getRootBone()->setPosition(0,0,0);
		mEntity->_updateAnimation();

		// place root bone at origin position
		Ogre::Vector3 dp = mEntity->getSkeleton()->getRootBone()->_getDerivedPosition();
		mNode->translate(-dp.x, 0, -dp.z);
		mEntity->getSkeleton()->getRootBone()->translate(-dp.x, 0, -dp.z);

    }
    return BaseApplication::keyPressed(arg);
}

bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent &evt) {
		
	Ogre::Real x1, x2, z1, z2;

    if (state == CALC_AVATAR_PATH)	// Run all animations of the model
	{
        // If the current time lenght <= total time lenght of the animation
        if(mAnimationState->getTimePosition() < mAnimationState->getLength() )
		{
			mAnimationState->addTime(evt.timeSinceLastFrame);
			//// Get current root bone position
			//Ogre::Vector3 rp = mEntity->getSkeleton()->getRootBone()->_getDerivedPosition();
			//// get current node position
			//Ogre::Vector3 np = mNode->getPosition();
			//// translate the root bone
			//mEntity->getSkeleton()->getRootBone()->translate(np.x+rp.x, 0,np.z+rp.z);
			//force animation update
			mEntity->_updateAnimation();
			// save this position 
			framePositionCollection.push_back(mEntity->getSkeleton()->getRootBone()->_getDerivedPosition());
        }

		// if the current animation has ended
        if(mAnimationState->getTimePosition() == mAnimationState->getLength() )
		{ 
			currentArcLenght = 0;
			// Calculate the total arclenght
			while(framePositionCollection.size() > 1)
			{
				// get two points, but only discard the 1st
				Ogre::Vector3 v1 = framePositionCollection.front();
				framePositionCollection.pop_front();
				Ogre::Vector3 v2 = framePositionCollection.front();

				currentArcLenght += (v2 - v1).length();
			}
			char str[50] = "";
			sprintf(str, "Arc Lenght: %.3f", currentArcLenght);
			mInfoLabel2->setCaption(str);

			// search for the vector in user defined path with the arcLenght 
			// if the number of points in the defined path is lower than the number os animations done
			if(mWalkList.size() > animationStep)
			{
				int vectIt = animationStep;
				
				s = mWalkList[vectIt-1];
				e = mWalkList[vectIt];
				Ogre::Vector3 v = e-s;
				Ogre::Real normaV = v.length();
				while(currentArcLenght > normaV )
				{
					vectIt++;
					if( vectIt == mWalkList.size() ) break;
					s = mWalkList[vectIt-1];
					e = mWalkList[vectIt];
					v = e-s;
					normaV += v.length();
				}

				// calculate any differences
				Ogre::Real dx, dp;
				dx = normaV - currentArcLenght;
				dp = v.length() - dx;

				v.normalise();
				m = s + dp*v;

				Ogre::Vector3 currentRootPos = mEntity->getSkeleton()->getRootBone()->_getDerivedPosition();

				currentAnimationError = (m - currentRootPos).length();

			}

			char str2[50];
			sprintf(str2, "Anim Error: %f", currentAnimationError);
			mInfoLabel3->setCaption(str2);

			anim_state = AVATAR_ANIM_DONE;
        }

		//this animation has ended, must check if there is another one to test
		if(anim_state == AVATAR_ANIM_DONE)	
		{
			if (assIte->hasMoreElements())
			{
				// check is this animation's arc lenght is a better match
				if(currentAnimationError <= choosenAnimation.second)
				{
					choosenAnimation.first = assIte->current()->second;
					choosenAnimation.second = currentAnimationError;
				}

				// get next animation 
				mAnimationState->setEnabled(false);
				mAnimationState->destroyBlendMask();
				mAnimationState = assIte->getNext();
				if(mAnimationState->getAnimationName() == "volta")
					mAnimationState = assIte->getNext();

				mAnimationState->setEnabled(true);
				mAnimationState->setLoop(false);
				mAnimationState->setTimePosition(0);
				framePositionCollection.clear();
				//anim_state = AVATAR_ANIM_IN_CALC;
				anim_state = CALC_AVATAR_PATH;
				
				Ogre::String stra = "Current animation is ";
				stra.append(mAnimationState->getAnimationName());
				mInfoLabel->setCaption(stra);
			}
			else // we have done all animations
			{
				// update how many animations we have done
				animationStep++;

				// commit the best animation to the set
				animationPath->push_back( choosenAnimation.first->getAnimationName() );

				// move avatar to the end of animation so that we can set the node to that position
				mAnimationState = choosenAnimation.first;
				mAnimationState->setEnabled(true);
				mAnimationState->setLoop(false);
				mAnimationState->addTime(mAnimationState->getLength());
				mEntity->_updateAnimation();
				
				// move the avatar to the point where the animation ended
				Ogre::Vector3 dp = mEntity->getSkeleton()->getRootBone()->_getDerivedPosition();
				mNode->translate(-dp.x, 0, -dp.z);

				// start over again
				//initPathSynthesis();

				// if user defined path has all been "animated" we must prepare to show the motion
				if( mWalkList.size() == animationStep )
				//if(anim_state != AVATAR_ANIM_IN_CALC)
				{
					state = SHOW_AVATAR_PATH;
					mAnimationState->setEnabled(false);
					mAnimationState->destroyBlendMask();
					
					// set the animation state be idle for a moment
					mAnimationState = mEntity->getAnimationState("idle");
					mAnimationState->setEnabled(true);
					mAnimationState->setLoop(false);
					mAnimationState->setTimePosition(0);
					
					//set the node and entity to origin position
					mNode->setPosition(0,0,0);
					mEntity->_updateAnimation();

					// place root bone at origin position
					Ogre::Vector3 dp = mEntity->getSkeleton()->getRootBone()->_getDerivedPosition();
					mNode->translate(-dp.x, 0, -dp.z);
					mEntity->getSkeleton()->getRootBone()->translate(-dp.x, 0, -dp.z);

					mInfoLabel->setCaption("Please hold a moment...");
					mInfoLabel2->hide();
					mInfoLabel3->hide();
					
					return true;
				}
			}
		}
	}

	// finally render the synthesized motion path
	if (state == SHOW_AVATAR_PATH) 
	{
		// if this animation reaches the end, start the next one
		if(mAnimationState->getTimePosition() == mAnimationState->getLength())
		{
			mAnimationState->setEnabled(false);
			mAnimationState->destroyBlendMask();
			
			// we still have animations to run
			if(animationPath->size())
			{
				// prepare the next animation to show
				mAnimationState = mEntity->getAnimationState( animationPath->front() );
				animationPath->pop_back();
				mAnimationState->setEnabled(true);
				mAnimationState->setLoop(false);
				mAnimationState->setTimePosition(0);

				// update avatar's position to the position where the animation ended
				// place root bone at origin position
				Ogre::Vector3 dp = mEntity->getSkeleton()->getRootBone()->_getDerivedPosition();
				mNode->translate(-dp.x, 0, -dp.z);
				mEntity->getSkeleton()->getRootBone()->translate(-dp.x, 0, -dp.z);

				Ogre::String strc = "Showing animation: ";
				strc.append(mAnimationState->getAnimationName());
				mInfoLabel->setCaption(strc);
			}
			else
			{
				state = AVATAR_PATH_DONE;
				mInfoLabel->setCaption("Calculated Motion Path has ended.\n");
			}
		}

		mAnimationState->addTime(evt.timeSinceLastFrame);
		mEntity->_updateAnimation();

	}

    return BaseApplication::frameRenderingQueued(evt);
}




#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
    int main(int argc, char *argv[])
#endif
    {
        // Create application object
        TutorialApplication app;

        try {
            app.go();
        } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
            MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
            std::cerr << "An exception has occured: " <<
                e.getFullDescription().c_str() << std::endl;
#endif
        }

        return 0;
    }

#ifdef __cplusplus
}
#endif
