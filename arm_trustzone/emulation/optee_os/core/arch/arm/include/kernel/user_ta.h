/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2015, Linaro Limited
 */
#ifndef KERNEL_USER_TA_H
#define KERNEL_USER_TA_H

#include <assert.h>
#include <kernel/tee_ta_manager.h>
#include <kernel/thread.h>
#include <mm/file.h>
#include <mm/tee_mm.h>
#include <scattered_array.h>
#include <tee_api_types.h>
#include <types_ext.h>
#include <util.h>
#include <utee_defines.h>

TAILQ_HEAD(tee_cryp_state_head, tee_cryp_state);
TAILQ_HEAD(tee_obj_head, tee_obj);
TAILQ_HEAD(tee_storage_enum_head, tee_storage_enum);
SLIST_HEAD(load_seg_head, load_seg);

/*
 * struct user_ta_ctx - user TA context
 * @entry_func:		Entry address in TA
 * @dump_entry_func:	Entry address in TA for dumping address mappings
 *			and stack trace
 * @ftrace_entry_func:	Entry address in ldelf for dumping ftrace data
 * @ldelf_stack_ptr:	Stack pointer used for dumping address mappings and
 *			stack trace
 * @is_32bit:		True if 32-bit TA, false if 64-bit TA
 * @is_initializing:	True if TA is not fully loaded
 * @open_sessions:	List of sessions opened by this TA
 * @cryp_states:	List of cryp states created by this TA
 * @objects:		List of storage objects opened by this TA
 * @storage_enums:	List of storage enumerators opened by this TA
 * @stack_ptr:		Stack pointer
 * @load_addr:		ELF load addr (from TA address space)
 * @vm_info:		Virtual memory map of this context
 * @ta_time_offs:	Time reference used by the TA
 * @areas:		Memory areas registered by pager
 * @vfp:		State of VFP registers
 * @ctx:		Generic TA context
 * @ta_image_sha256:	Fingerprint of the TA, based on a hash of all loaded
 *			ELFs which make up the TA
 */
struct user_ta_ctx {
	uaddr_t entry_func;
	uaddr_t dump_entry_func;
#ifdef CFG_TA_FTRACE_SUPPORT
	uaddr_t ftrace_entry_func;
#endif
	uaddr_t ldelf_stack_ptr;
	bool is_32bit;
	bool is_initializing;
	struct tee_ta_session_head open_sessions;
	struct tee_cryp_state_head cryp_states;
	struct tee_obj_head objects;
	struct tee_storage_enum_head storage_enums;
	vaddr_t stack_ptr;
	vaddr_t load_addr;
	struct vm_info *vm_info;
	void *ta_time_offs;
	struct tee_pager_area_head *areas;
#ifdef CFG_ATTESTATION_MEASURE
	uint8_t ta_image_sha256[TEE_SHA256_HASH_SIZE];
#endif
	/*
	 * Note that the load segments are stored in reverse order, that
	 * is, the last segment first.
	 */
	struct load_seg_head segs;
#if defined(CFG_WITH_VFP)
	struct thread_user_vfp_state vfp;
#endif
	struct tee_ta_ctx ctx;

};

#ifdef CFG_WITH_USER_TA
bool is_user_ta_ctx(struct tee_ta_ctx *ctx);
#else
static inline bool is_user_ta_ctx(struct tee_ta_ctx *ctx __unused)
{
	return false;
}
#endif

static inline struct user_ta_ctx *to_user_ta_ctx(struct tee_ta_ctx *ctx)
{
	assert(is_user_ta_ctx(ctx));
	return container_of(ctx, struct user_ta_ctx, ctx);
}

struct user_ta_store_ops;

#ifdef CFG_WITH_USER_TA
TEE_Result tee_ta_init_user_ta_session(const TEE_UUID *uuid,
			struct tee_ta_session *s);
#else
static inline TEE_Result tee_ta_init_user_ta_session(
			const TEE_UUID *uuid __unused,
			struct tee_ta_session *s __unused)
{
	return TEE_ERROR_ITEM_NOT_FOUND;
}
#endif

struct fobj;
#ifdef CFG_WITH_USER_TA
TEE_Result user_ta_map(struct user_ta_ctx *utc, vaddr_t *va, struct fobj *f,
		       uint32_t prot, struct file *file, size_t pad_begin,
		       size_t pad_end);
#else
static inline TEE_Result user_ta_map(struct user_ta_ctx *utc __unused,
				     vaddr_t *va __unused,
				     struct fobj *f __unused,
				     uint32_t prot __unused,
				     struct file *file __unused,
				     size_t pad_begin __unused,
				     size_t pad_end __unused)
{
	return TEE_ERROR_GENERIC;
}
#endif

#ifdef CFG_WITH_USER_TA
TEE_Result user_ta_unmap(struct user_ta_ctx *utc, vaddr_t va, size_t len);
#else
static inline TEE_Result user_ta_unmap(struct user_ta_ctx *utc __unused,
				       vaddr_t va __unused, size_t len __unused)
{
	return TEE_ERROR_GENERIC;
}
#endif

#ifdef CFG_WITH_USER_TA
TEE_Result user_ta_set_prot(struct user_ta_ctx *utc, vaddr_t va, size_t len,
			    uint32_t prot);
#else
static inline TEE_Result user_ta_set_prot(struct user_ta_ctx *utc __unused,
					  vaddr_t va __unused,
					  size_t len __unused,
					  uint32_t prot __unused)
{
	return TEE_ERROR_GENERIC;
}
#endif

#ifdef CFG_WITH_USER_TA
TEE_Result user_ta_remap(struct user_ta_ctx *utc, vaddr_t *new_va,
			 vaddr_t old_va, size_t len, size_t pad_begin,
			 size_t pad_end);
#else
static inline TEE_Result user_ta_remap(struct user_ta_ctx *utc __unused,
				       vaddr_t *new_va __unused,
				       vaddr_t old_va __unused,
				       size_t len __unused,
				       size_t pad_begin __unused,
				       size_t pad_end __unused);
{
	return TEE_ERROR_GENERIC;
}
#endif

/*
 * Registers a TA storage.
 *
 * A TA is loaded from the first TA storage in which the TA can be found.
 * TA storage is searched in order of priority, where lower values are
 * tried first.
 *
 * Note prio must be unique per storage in order to avoid dependency on
 * registration order. This is enforced by a deliberate linker error in
 * case of conflict.
 *
 * Also note that TA storage is sorted lexicographically instead of
 * numerically.
 */
#define TEE_TA_REGISTER_TA_STORE(prio) \
	int __tee_ta_store_##prio __unused; \
	SCATTERED_ARRAY_DEFINE_PG_ITEM_ORDERED(ta_stores, prio, \
					       struct user_ta_store_ops)

#endif /*KERNEL_USER_TA_H*/
