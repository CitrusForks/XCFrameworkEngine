/* XCFrameworkEngine
 * Copyright (C) Abhishek Porwal, 2016
 * Any queries? Contact author <https://github.com/abhishekp314>
 * This program is complaint with GNU General Public License, version 3.
 * For complete license, read License.txt in source root directory. */

#ifndef AV_ANIMEVALUATOR_H_INCLUDED
#define AV_ANIMEVALUATOR_H_INCLUDED

#include "AssimpIncludes.h"

/** Calculates transformations for a given timestamp from a set of animation tracks. Not directly useful,
 * better use the AnimPlayer class.
 */
class AnimEvaluator
{
public:

#if USE_CUSTOM_TUPLE
    template<class T1, class T2, class T3>
    struct tuple
    {
        tuple(T1 v1, T2 v2, T3 v3)
            : val1(v1), val2(v2), val3(v3)
        {}

        template<int index>
        unsigned int get()
        {
            switch (index)
            {
            case 0: return val1;
            case 1: return val2;
            case 2: return val3;
            default: return 0;
            }
        }

        template<int index>
        void set(unsigned int value)
        {
            switch (index)
            {
            case 0: val1 = value;
            case 1: val2 = value;
            case 2: val3 = value;
            default: return;
            }
        }
            

        T1 val1;
        T2 val2;
        T3 val3;
    };

#endif

    /** Constructor on a given animation. The animation is fixed throughout the lifetime of
     * the object.
     * @param pAnim The animation to calculate poses for. Ownership of the animation object stays
     *   at the caller, the evaluator just keeps a reference to it as long as it persists.
     */
    AnimEvaluator( const aiAnimation* pAnim);

    /** Evaluates the animation tracks for a given time stamp. The calculated pose can be retrieved as a
     * array of transformation matrices afterwards by calling GetTransformations().
     * @param pTime The time for which you want to evaluate the animation, in seconds. Will be mapped into the animation cycle, so
     *   it can be an arbitrary value. Best use with ever-increasing time stamps.
     */
    void Evaluate( double pTime);

    /** Returns the transform matrices calculated at the last Evaluate() call. The array matches the mChannels array of
     * the aiAnimation. */
    const std::vector<aiMatrix4x4>& GetTransformations() const { return mTransforms; }

protected:
    /** The animation we're working on */
    const aiAnimation* mAnim;

    /** At which frame the last evaluation happened for each channel.
     * Useful to quickly find the corresponding frame for slightly increased time stamps
     */
    double mLastTime;
    std::vector<boost::tuple<unsigned int, unsigned int, unsigned int> > mLastPositions;

    /** The array to store the transformations results of the evaluation */
    std::vector<aiMatrix4x4> mTransforms;
};

#endif // AV_ANIMEVALUATOR_H_INCLUDED
