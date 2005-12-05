/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: Ptr.cpp,v $
//
//   $Revision: 1.7 $
//
// Description: reference counting smart pointer
//              based on shared_ptr from www.boost.org
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Ptr.h"

using namespace asl;

asl::AtomicCount<asl::MultiProcessor>  asl::SpinLockedPtrFreeListAllocator::_theThreadLock_(1);
// Q: why is this volatile? (MS & DS) A: To avoid multiprocessor cache coherence problems (PM)
asl::ReferenceCounter<asl::MultiProcessor> * volatile asl::SpinLockedPtrFreeListAllocator::_theFreeListHead_ = 0;

asl::ReferenceCounter<PtrFreeListAllocator::ThreadingModel> * asl::PtrFreeListAllocator::_theFreeListHead_ = 0;

template <>
asl::ReferenceCounter<SingleThreaded> * asl::PtrFreeListChunkAllocator<SingleThreaded>::_theFreeListHead_ = 0;
template <>
asl::ReferenceCounter<SingleProcessor> * asl::PtrFreeListChunkAllocator<SingleProcessor>::_theFreeListHead_ = 0;
template <>
asl::ReferenceCounter<MultiProcessor> * asl::PtrFreeListChunkAllocator<MultiProcessor>::_theFreeListHead_ = 0;

template <>
pthread_key_t asl::PtrThreadSpecificFreeListAllocator<SingleThreaded>::_theKey_ = 0;
template <>
pthread_key_t asl::PtrThreadSpecificFreeListAllocator<asl::SingleProcessor>::_theKey_ = 0;
template <>
pthread_key_t asl::PtrThreadSpecificFreeListAllocator<MultiProcessor>::_theKey_ = 0;

pthread_key_t asl::PtrThreadSpecificFreeListChunkAllocator::_theKey_ = 0;

pthread_mutex_t asl::MutexPtrFreeListAllocator::_theMutex_;
asl::ReferenceCounter<asl::MultiProcessor> * asl::MutexPtrFreeListAllocator::_theFreeListHead_ = 0;

asl::ReferenceCounter<asl::MultiProcessor> * asl::PtrMultiFreeListAllocator::_theFreeListHead_[asl::PtrMultiFreeListAllocator::NUM_LISTS] =
{ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
asl::AtomicCount<asl::MultiProcessor> asl::PtrMultiFreeListAllocator::_theIndex_(0);


