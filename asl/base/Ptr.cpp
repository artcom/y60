/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
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

// own header
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


