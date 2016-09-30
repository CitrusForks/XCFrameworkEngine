/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#include "GraphicsPrecompiledHeader.h"
#include "AnimEvaluator.h"

// ------------------------------------------------------------------------------------------------
// Constructor on a given animation.
AnimEvaluator::AnimEvaluator( const aiAnimation* pAnim)
{
    mAnim = pAnim;
    mLastTime = 0.0;
    mLastPositions.resize( pAnim->mNumChannels, boost::make_tuple( 0, 0, 0));
}

// ------------------------------------------------------------------------------------------------
// Evaluates the animation tracks for a given time stamp.
void AnimEvaluator::Evaluate( f64 pTime)
{
    // extract ticks per second. Assume default value if not given
    f64 ticksPerSecond = mAnim->mTicksPerSecond != 0.0 ? mAnim->mTicksPerSecond : 25.0;
    // every following time calculation happens in ticks
    pTime *= ticksPerSecond;

    // map into anim's duration
    f64 time = 0.0f;
    if( mAnim->mDuration > 0.0)
        time = fmod( pTime, mAnim->mDuration);

    if( mTransforms.size() != mAnim->mNumChannels)
        mTransforms.resize( mAnim->mNumChannels);

    // calculate the transformations for each animation channel
    for( u32 a = 0; a < mAnim->mNumChannels; a++)
    {
        const aiNodeAnim* channel = mAnim->mChannels[a];

        // ******** Position *****
        aiVector3D presentPosition( 0, 0, 0);
        if( channel->mNumPositionKeys > 0)
        {
            // Look for present frame number. Search from last position if time is after the last time, else from beginning
            // Should be much quicker than always looking from start for the average use case.
            u32 frame = (time >= mLastTime) ? mLastPositions[a].get<0>() : 0;
            while( frame < channel->mNumPositionKeys - 1)
            {
                if( time < channel->mPositionKeys[frame+1].mTime)
                    break;
                frame++;
            }

            // interpolate between this frame's value and next frame's value
            u32 nextFrame = (frame + 1) % channel->mNumPositionKeys;
            const aiVectorKey& key = channel->mPositionKeys[frame];
            const aiVectorKey& nextKey = channel->mPositionKeys[nextFrame];
            f64 diffTime = nextKey.mTime - key.mTime;
            if( diffTime < 0.0)
                diffTime += mAnim->mDuration;
            if( diffTime > 0)
            {
                f32 factor = f32( (time - key.mTime) / diffTime);
                presentPosition = key.mValue + (nextKey.mValue - key.mValue) * factor;
            } else
            {
                presentPosition = key.mValue;
            }

            mLastPositions[a].get<0>() = frame;
        }

        // ******** Rotation *********
        aiQuaternion presentRotation( 1, 0, 0, 0);
        if( channel->mNumRotationKeys > 0)
        {
            u32 frame = (time >= mLastTime) ? mLastPositions[a].get<1>() : 0;
            while( frame < channel->mNumRotationKeys - 1)
            {
                if( time < channel->mRotationKeys[frame+1].mTime)
                    break;
                frame++;
            }

            // interpolate between this frame's value and next frame's value
            u32 nextFrame = (frame + 1) % channel->mNumRotationKeys;
            const aiQuatKey& key = channel->mRotationKeys[frame];
            const aiQuatKey& nextKey = channel->mRotationKeys[nextFrame];
            f64 diffTime = nextKey.mTime - key.mTime;
            if( diffTime < 0.0)
                diffTime += mAnim->mDuration;
            if( diffTime > 0)
            {
                f32 factor = f32( (time - key.mTime) / diffTime);
                aiQuaternion::Interpolate( presentRotation, key.mValue, nextKey.mValue, factor);
            } else
            {
                presentRotation = key.mValue;
            }

            mLastPositions[a].get<1>() = frame;
        }

        // ******** Scaling **********
        aiVector3D presentScaling( 1, 1, 1);
        if( channel->mNumScalingKeys > 0)
        {
            u32 frame = (time >= mLastTime) ? mLastPositions[a].get<2>() : 0;
            while( frame < channel->mNumScalingKeys - 1)
            {
                if( time < channel->mScalingKeys[frame+1].mTime)
                    break;
                frame++;
            }

            // TODO: (thom) interpolation maybe? This time maybe even logarithmic, not linear
            presentScaling = channel->mScalingKeys[frame].mValue;
            mLastPositions[a].get<2>() = frame;
        }

        // build a transformation matrix from it
        aiMatrix4x4& mat = mTransforms[a];
        mat = aiMatrix4x4( presentRotation.GetMatrix());
        mat.a1 *= presentScaling.x; mat.b1 *= presentScaling.x; mat.c1 *= presentScaling.x;
        mat.a2 *= presentScaling.y; mat.b2 *= presentScaling.y; mat.c2 *= presentScaling.y;
        mat.a3 *= presentScaling.z; mat.b3 *= presentScaling.z; mat.c3 *= presentScaling.z;
        mat.a4 = presentPosition.x; mat.b4 = presentPosition.y; mat.c4 = presentPosition.z;
        //mat.Transpose();
    }

    mLastTime = time;
}
