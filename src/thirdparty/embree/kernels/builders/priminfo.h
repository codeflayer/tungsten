// ======================================================================== //
// Copyright 2009-2016 Intel Corporation                                    //
//                                                                          //
// Licensed under the Apache License, Version 2.0 (the "License");          //
// you may not use this file except in compliance with the License.         //
// You may obtain a copy of the License at                                  //
//                                                                          //
//     http://www.apache.org/licenses/LICENSE-2.0                           //
//                                                                          //
// Unless required by applicable law or agreed to in writing, software      //
// distributed under the License is distributed on an "AS IS" BASIS,        //
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. //
// See the License for the specific language governing permissions and      //
// limitations under the License.                                           //
// ======================================================================== //

#pragma once

#include "../common/default.h"

namespace embree
{
  namespace isa
  {
    class CentGeomBBox3fa
    {
    public:
      __forceinline CentGeomBBox3fa () {}

      __forceinline CentGeomBBox3fa (EmptyTy) 
	: geomBounds(empty), centBounds(empty) {}
      
      __forceinline CentGeomBBox3fa (const BBox3fa& geomBounds, const BBox3fa& centBounds) 
	: geomBounds(geomBounds), centBounds(centBounds) {}
      
      __forceinline void extend(const BBox3fa& geomBounds_, const BBox3fa& centBounds_) {
	geomBounds.extend(geomBounds_);
	centBounds.extend(centBounds_);
      }

      __forceinline void reset() {
	geomBounds = empty;
	centBounds = empty;
      }

      __forceinline void extend(const BBox3fa& geomBounds_) {
	geomBounds.extend(geomBounds_);
	centBounds.extend(center2(geomBounds_));
      }

      __forceinline void merge(const CentGeomBBox3fa& other) 
      {
	geomBounds.extend(other.geomBounds);
	centBounds.extend(other.centBounds);
      }
      
    public:
      BBox3fa geomBounds;   //!< geometry bounds of primitives
      BBox3fa centBounds;   //!< centroid bounds of primitives
    };

    /*! stores bounding information for a set of primitives */
    class PrimInfo : public CentGeomBBox3fa
    {
    public:
      __forceinline PrimInfo () {}

      __forceinline PrimInfo (EmptyTy) 
	: CentGeomBBox3fa(empty), begin(0), end(0) {}

      __forceinline void reset() {
	CentGeomBBox3fa::reset();
	begin = end;
      }
      
      __forceinline PrimInfo (size_t num, const BBox3fa& geomBounds, const BBox3fa& centBounds) 
	: CentGeomBBox3fa(geomBounds,centBounds), begin(0), end(num) {}
      
      __forceinline PrimInfo (size_t begin, size_t end, const BBox3fa& geomBounds, const BBox3fa& centBounds) 
	: CentGeomBBox3fa(geomBounds,centBounds), begin(begin), end(end) {}

      __forceinline void add(const BBox3fa& geomBounds_) {
	CentGeomBBox3fa::extend(geomBounds_,center2(geomBounds_));
	end++;
      }

      __forceinline void add(const BBox3fa& geomBounds_, const size_t i) {
	CentGeomBBox3fa::extend(geomBounds_,center2(geomBounds_));
	end+=i;
      }

      __forceinline void add(const size_t i=1) {
	end+=i;
      }
      
      __forceinline void add(const BBox3fa& geomBounds_, const BBox3fa& centBounds_, size_t num_ = 1) {
	CentGeomBBox3fa::extend(geomBounds_,centBounds_);
	end += num_;
      }

      __forceinline void merge(const PrimInfo& other) 
      {
	CentGeomBBox3fa::merge(other);
	//assert(begin == 0);
        begin += other.begin;
	end += other.end;
      }

      static __forceinline const PrimInfo merge(const PrimInfo& a, const PrimInfo& b) {
        PrimInfo r = a; r.merge(b); return r;
      }
      
      /*! returns the number of primitives */
      __forceinline size_t size() const { 
	return end-begin; 
      }
      
      __forceinline float leafSAH() const { 
	return halfArea(geomBounds)*float(size()); 
	//return halfArea(geomBounds)*blocks(num); 
      }
      
      __forceinline float leafSAH(size_t block_shift) const { 
	return halfArea(geomBounds)*float((size()+(size_t(1)<<block_shift)-1) >> block_shift);
	//return halfArea(geomBounds)*float((num+3) >> 2);
	//return halfArea(geomBounds)*blocks(num); 
      }
      
      /*! stream output */
      friend std::ostream& operator<<(std::ostream& cout, const PrimInfo& pinfo) {
	return cout << "PrimInfo { begin = " << pinfo.begin << ", end = " << pinfo.end << ", geomBounds = " << pinfo.geomBounds << ", centBounds = " << pinfo.centBounds << "}";
      }
      
    public:
      size_t begin,end;          //!< number of primitives
    };

    struct PrimInfo2 
    {
      __forceinline PrimInfo2() {}
      
      __forceinline PrimInfo2(EmptyTy) 
        : left(empty), right(empty) {}
      
      __forceinline PrimInfo2(const PrimInfo& left, const PrimInfo& right)
        : left(left), right(right) {}
      
      static __forceinline const PrimInfo2 merge (const PrimInfo2& a, const PrimInfo2& b) {
        return PrimInfo2(PrimInfo::merge(a.left,b.left),PrimInfo::merge(a.right,b.right));
      }
      
    public:
      PrimInfo left,right;
    };



  }
}
