/*
 * Copyright © 2008 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice (including the next
 * paragraph) shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 *
 * Authors:
 *    Eric Anholt <eric@anholt.net>
 *
 */

/**
 * @file intel_bufmgr_priv.h
 *
 * Private definitions of Intel-specific bufmgr functions and structures.
 */

#ifndef INTEL_BUFMGR_PRIV_H
#define INTEL_BUFMGR_PRIV_H

/**
 * Context for a buffer manager instance.
 *
 * Contains public methods followed by private storage for the buffer manager.
 */
struct _dri_bufmgr {
   /**
    * Allocate a buffer object.
    *
    * Buffer objects are not necessarily initially mapped into CPU virtual
    * address space or graphics device aperture.  They must be mapped using
    * bo_map() to be used by the CPU, and validated for use using bo_validate()
    * to be used from the graphics device.
    */
   dri_bo *(*bo_alloc)(dri_bufmgr *bufmgr_ctx, const char *name,
		       unsigned long size, unsigned int alignment);

   /** Takes a reference on a buffer object */
   void (*bo_reference)(dri_bo *bo);

   /**
    * Releases a reference on a buffer object, freeing the data if
    * rerefences remain.
    */
   void (*bo_unreference)(dri_bo *bo);

   /**
    * Maps the buffer into userspace.
    *
    * This function will block waiting for any existing execution on the
    * buffer to complete, first.  The resulting mapping is available at
    * buf->virtual.
    */
   int (*bo_map)(dri_bo *buf, int write_enable);

   /** Reduces the refcount on the userspace mapping of the buffer object. */
   int (*bo_unmap)(dri_bo *buf);

   /**
    * Write data into an object.
    *
    * This is an optional function, if missing,
    * dri_bo will map/memcpy/unmap.
    */
   int (*bo_subdata) (dri_bo *buf, unsigned long offset,
		      unsigned long size, const void *data);

   /**
    * Read data from an object
    *
    * This is an optional function, if missing,
    * dri_bo will map/memcpy/unmap.
    */
   int (*bo_get_subdata) (dri_bo *bo, unsigned long offset,
			  unsigned long size, void *data);

   /**
    * Waits for rendering to an object by the GPU to have completed.
    *
    * This is not required for any access to the BO by bo_map, bo_subdata, etc.
    * It is merely a way for the driver to implement glFinish.
    */
   void (*bo_wait_rendering) (dri_bo *bo);

   /**
    * Tears down the buffer manager instance.
    */
   void (*destroy)(dri_bufmgr *bufmgr);

    /**
     * Add relocation entry in reloc_buf, which will be updated with the
     * target buffer's real offset on on command submission.
     *
     * Relocations remain in place for the lifetime of the buffer object.
     *
     * \param reloc_buf Buffer to write the relocation into.
     * \param read_domains GEM read domains which the buffer will be read into
     *	      by the command that this relocation is part of.
     * \param write_domains GEM read domains which the buffer will be dirtied
     *	      in by the command that this relocation is part of.
     * \param delta Constant value to be added to the relocation target's
     *	       offset.
     * \param offset Byte offset within batch_buf of the relocated pointer.
     * \param target Buffer whose offset should be written into the relocation
     *	     entry.
     */
    int (*bo_emit_reloc)(dri_bo *reloc_buf,
			 uint32_t read_domains, uint32_t write_domain,
			 uint32_t delta, uint32_t offset, dri_bo *target);

    /** Executes the command buffer pointed to by bo. */
    int (*bo_exec)(dri_bo *bo, int used,
		   drm_clip_rect_t *cliprects, int num_cliprects,
		   int DR4);

    /**
     * Pin a buffer to the aperture and fix the offset until unpinned
     *
     * \param buf Buffer to pin
     * \param alignment Required alignment for aperture, in bytes
     */
    int (*bo_pin) (dri_bo *buf, uint32_t alignment);
    /**
     * Unpin a buffer from the aperture, allowing it to be removed
     *
     * \param buf Buffer to unpin
     */
    int (*bo_unpin) (dri_bo *buf);
    /**
     * Ask that the buffer be placed in tiling mode
     *
     * \param buf Buffer to set tiling mode for
     * \param tiling_mode desired, and returned tiling mode
     */
    int (*bo_set_tiling) (dri_bo *bo, uint32_t *tiling_mode);
    /**
     * Get the current tiling (and resulting swizzling) mode for the bo.
     *
     * \param buf Buffer to get tiling mode for
     * \param tiling_mode returned tiling mode
     * \param swizzle_mode returned swizzling mode
     */
    int (*bo_get_tiling) (dri_bo *bo, uint32_t *tiling_mode,
			  uint32_t *swizzle_mode);
    /**
     * Create a visible name for a buffer which can be used by other apps
     *
     * \param buf Buffer to create a name for
     * \param name Returned name
     */
    int (*bo_flink) (dri_bo *buf, uint32_t *name);

    int (*check_aperture_space)(dri_bo **bo_array, int count);
    int debug; /**< Enables verbose debugging printouts */
};

#endif /* INTEL_BUFMGR_PRIV_H */
