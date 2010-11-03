/*************************************************************************
Copyright (c) Sergey Bochkanov (ALGLIB project).

>>> SOURCE LICENSE >>>
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation (www.fsf.org); either version 2 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

A copy of the GNU General Public License is available at
http://www.fsf.org/licensing/licenses
>>> END OF LICENSE >>>
*************************************************************************/
#include "stdafx.h"
#include "ap.h"
#include <limits>
#include <locale.h>

// disable some irrelevant warnings
#if (AE_COMPILER==AE_MSVC)
#pragma warning(disable:4100)
#pragma warning(disable:4127)
#pragma warning(disable:4702)
#pragma warning(disable:4996)
#endif

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION IMPLEMENTS BASIC FUNCTIONALITY LIKE
// MEMORY MANAGEMENT FOR VECTORS/MATRICES WHICH IS
// SHARED BETWEEN C++ AND PURE C LIBRARIES
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
/*
 * local definitions
 */
#define x_nb 16
#define AE_DATA_ALIGN 16
#define AE_PTR_ALIGN sizeof(void*)
#define DYN_BOTTOM ((void*)1)
#define DYN_FRAME  ((void*)2)
#define AE_LITTLE_ENDIAN 1
#define AE_BIG_ENDIAN 2
#define AE_MIXED_ENDIAN 3


/*
 * alloc counter (if used)
 */
#ifdef AE_USE_ALLOC_COUNTER
ae_int64_t _alloc_counter = 0;
#endif

/*
 * These declarations are used to ensure that
 * sizeof(ae_int32_t)==4, sizeof(ae_int64_t)==8, sizeof(ae_int_t)==sizeof(void*).
 * they will lead to syntax error otherwise (array size will be negative).
 *
 * you can remove them, if you want - they are not used anywhere.
 *
 */
static char _ae_int32_t_must_be_32_bits_wide[1-2*((int)(sizeof(ae_int32_t))-4)*((int)(sizeof(ae_int32_t))-4)];
static char _ae_int64_t_must_be_64_bits_wide[1-2*((int)(sizeof(ae_int64_t))-8)*((int)(sizeof(ae_int64_t))-8)];
static char _ae_int_t_must_be_pointer_sized [1-2*((int)(sizeof(ae_int_t))-(int)sizeof(void*))*((int)(sizeof(ae_int_t))-(int)(sizeof(void*)))];  

void* ae_align(void *ptr, size_t alignment)
{
    char *result = (char*)ptr;
    if( (result-(char*)0)%alignment!=0 )
        result += alignment - (result-(char*)0)%alignment;
    return result;
}

void ae_break(ae_state *state, ae_error_type error_type, const char *msg)
{
#ifndef AE_USE_CPP_ERROR_HANDLING
    if( state!=NULL )
    {
        ae_state_clear(state);
        state->last_error = error_type;
        state->error_msg = msg;
        if( state->break_jump!=NULL )
            longjmp(*(state->break_jump), 1);
        else
            abort();
    }
    else
        abort();
#else
    if( state!=NULL )
    {
        ae_state_clear(state);
        state->last_error = error_type;
        state->error_msg = msg;
    }
    throw error_type;
#endif
}

void* aligned_malloc(size_t size, size_t alignment)
{
    if( size==0 )
        return NULL;
    if( alignment<=1 )
    {
        /* no alignment, just call malloc */
        void *block;
        void **p; ;
        block = malloc(sizeof(void*)+size);
        if( block==NULL )
            return NULL;
        p = (void**)block;
        *p = block;
#ifdef AE_USE_ALLOC_COUNTER
        _alloc_counter++;
#endif
        return (void*)((char*)block+sizeof(void*));
    }
    else
    {
        /* align */
        void *block;
        char *result;
        block = malloc(alignment-1+sizeof(void*)+size);
        if( block==NULL )
            return NULL;
        result = (char*)block+sizeof(void*);
        /*if( (result-(char*)0)%alignment!=0 )
            result += alignment - (result-(char*)0)%alignment;*/
        result = (char*)ae_align(result, alignment);
        *((void**)(result-sizeof(void*))) = block;
#ifdef AE_USE_ALLOC_COUNTER
        _alloc_counter++;
#endif
        return result;
    }
}

void aligned_free(void *block)
{
    void *p;
    if( block==NULL )
        return;
    p = *((void**)((char*)block-sizeof(void*)));
    free(p);
#ifdef AE_USE_ALLOC_COUNTER
    _alloc_counter--;
#endif
}

/************************************************************************
Malloc's memory with automatic alignment.

Returns NULL when zero size is specified.

Error handling:
* if state is NULL, returns NULL on allocation error
* if state is not NULL, calls ae_break() on allocation error
************************************************************************/
void* ae_malloc(size_t size, ae_state *state)
{
    void *result;
    if( size==0 )
        return NULL;
    result = aligned_malloc(size,AE_DATA_ALIGN);
    if( result==NULL && state!=NULL)
        ae_break(state, ERR_OUT_OF_MEMORY, "ae_malloc(): out of memory");
    return result;
}

void ae_free(void *p)
{
    if( p!=NULL )
        aligned_free(p);
}

/************************************************************************
Sets pointers to the matrix rows.

* dst must be correctly initialized matrix
* dst->data.ptr points to the beginning of memory block  allocated  for  
  row pointers.
* dst->ptr - undefined (initialized during algorithm processing)
* storage parameter points to the beginning of actual storage
************************************************************************/
void ae_matrix_update_row_pointers(ae_matrix *dst, void *storage)
{
    char *p_base;
    void **pp_ptr;
    ae_int_t i;
    if( dst->rows>0 && dst->cols>0 )
    {
        p_base = (char*)storage;
        pp_ptr = (void**)dst->data.ptr;
        dst->ptr.pp_void = pp_ptr;
        for(i=0; i<dst->rows; i++, p_base+=dst->stride*ae_sizeof(dst->datatype))
            pp_ptr[i] = p_base;
    }
    else
        dst->ptr.pp_void = NULL;
}

/************************************************************************
Returns size of datatype.
Zero for dynamic types like strings or multiple precision types.
************************************************************************/
ae_int_t ae_sizeof(ae_datatype datatype)
{
    switch(datatype)
    {
        case DT_BOOL:       return 1;
        case DT_INT:        return (ae_int_t)sizeof(ae_int_t);
        case DT_REAL:       return (ae_int_t)sizeof(double);
        case DT_COMPLEX:    return 2*(ae_int_t)sizeof(double);
        default:            return 0;
    }
}

/************************************************************************
This function initializes ALGLIB environment state.

NOTES:
* stacks contain no frames, so ae_make_frame() must be called before
  attaching dynamic blocks. Without it ae_leave_frame() will cycle
  forever (which is intended behavior).
************************************************************************/
void ae_state_init(ae_state *state)
{
    ae_int32_t *vp;

    /*
     * p_next points to itself because:
     * * correct program should be able to detect end of the list
     *   by looking at the ptr field.
     * * NULL p_next may be used to distinguish automatic blocks
     *   (in the list) from non-automatic (not in the list)
     */
    state->last_block.p_next = &(state->last_block);
    state->last_block.deallocator = NULL;
    state->last_block.ptr = DYN_BOTTOM;
    state->p_top_block = &(state->last_block);
    state->break_jump = NULL;
    state->error_msg = "";
    
    /*
     * determine endianness and initialize precomputed IEEE special quantities.
     */
    state->endianness = ae_get_endianness();
    if( state->endianness==AE_LITTLE_ENDIAN )
    {
        vp = (ae_int32_t*)(&state->v_nan);
        vp[0] = 0;
        vp[1] = (ae_int32_t)0x7FF80000;
        vp = (ae_int32_t*)(&state->v_posinf);
        vp[0] = 0;
        vp[1] = (ae_int32_t)0x7FF00000;
        vp = (ae_int32_t*)(&state->v_neginf);
        vp[0] = 0;
        vp[1] = (ae_int32_t)0xFFF00000;
    }
    else if( state->endianness==AE_BIG_ENDIAN )
    {
        vp = (ae_int32_t*)(&state->v_nan);
        vp[1] = 0;
        vp[0] = (ae_int32_t)0x7FF80000;
        vp = (ae_int32_t*)(&state->v_posinf);
        vp[1] = 0;
        vp[0] = (ae_int32_t)0x7FF00000;
        vp = (ae_int32_t*)(&state->v_neginf);
        vp[1] = 0;
        vp[0] = (ae_int32_t)0xFFF00000;
    }
    else
        abort();
}


/************************************************************************
This function clears ALGLIB environment state.
All dynamic data controlled by state are freed.
************************************************************************/
void ae_state_clear(ae_state *state)
{
    while( state->p_top_block->ptr!=DYN_BOTTOM )
        ae_frame_leave(state);
}


/************************************************************************
This function sets jump buffer for error handling.

buf may be NULL.
************************************************************************/
void ae_state_set_break_jump(ae_state *state, jmp_buf *buf)
{
    state->break_jump = buf;
}


/************************************************************************
This function makes new stack frame.

This function takes two parameters: environment state and pointer to  the
dynamic block which will be used as indicator  of  the  frame  beginning.
This dynamic block must be initialized by caller and mustn't  be changed/
deallocated/reused till ae_leave_frame called. It may be global or  local
variable (local is even better).
************************************************************************/
void ae_frame_make(ae_state *state, ae_frame *tmp)
{
    tmp->db_marker.p_next = state->p_top_block;
    tmp->db_marker.deallocator = NULL;
    tmp->db_marker.ptr = DYN_FRAME;
    state->p_top_block = &tmp->db_marker;
}


/************************************************************************
This function leaves current stack frame and deallocates all automatic
dynamic blocks which were attached to this frame.
************************************************************************/
void ae_frame_leave(ae_state *state)
{
    while( state->p_top_block->ptr!=DYN_FRAME && state->p_top_block->ptr!=DYN_BOTTOM)
    {
        if( state->p_top_block->ptr!=NULL && state->p_top_block->deallocator!=NULL)
            ((ae_deallocator)(state->p_top_block->deallocator))(state->p_top_block->ptr);
        state->p_top_block = state->p_top_block->p_next;
    }
    state->p_top_block = state->p_top_block->p_next;
}


/************************************************************************
This function attaches block to the dynamic block list

block               block
state               ALGLIB environment state

NOTES:
* never call it for special blocks which marks frame boundaries!
************************************************************************/
void ae_db_attach(ae_dyn_block *block, ae_state *state)
{
    block->p_next = state->p_top_block;
    state->p_top_block = block;
}


/************************************************************************
This function malloc's dynamic block:

block               destination block, assumed to be uninitialized
size                size (in bytes)
state               ALGLIB environment state. May be NULL.
make_automatic      if true, vector is added to the dynamic block list

block is assumed to be uninitialized, its fields are ignored.

Error handling:
* if state is NULL, returns ae_false on allocation error
* if state is not NULL, calls ae_break() on allocation error
* returns ae_true on success

NOTES:
* never call it for blocks which are already in the list
************************************************************************/
ae_bool ae_db_malloc(ae_dyn_block *block, ae_int_t size, ae_state *state, ae_bool make_automatic)
{
    /* ensure that size is >=0
       two ways to exit: 1) through ae_assert, if we have non-NULL state, 2) by returning ae_false */
    if( state!=NULL )
        ae_assert(size>=0, "ae_db_malloc(): negative size", state);
    if( size<0 )
        return ae_false;
    
    /* alloc */
    block->ptr = ae_malloc((size_t)size, state);
    if( block->ptr==NULL && size!=0 )
        return ae_false;
    if( make_automatic && state!=NULL )
        ae_db_attach(block, state);
    else
        block->p_next = NULL;
    block->deallocator = ae_free;
    return ae_true;
}


/************************************************************************
This function realloc's dynamic block:

block               destination block (initialized)
size                new size (in bytes)
state               ALGLIB environment state

block is assumed to be initialized.

This function:
* deletes old contents
* preserves automatic state

Error handling:
* if state is NULL, returns ae_false on allocation error
* if state is not NULL, calls ae_break() on allocation error
* returns ae_true on success

NOTES:
* never call it for special blocks which mark frame boundaries!
************************************************************************/
ae_bool ae_db_realloc(ae_dyn_block *block, ae_int_t size, ae_state *state)
{
    /* ensure that size is >=0
       two ways to exit: 1) through ae_assert, if we have non-NULL state, 2) by returning ae_false */
    if( state!=NULL )
        ae_assert(size>=0, "ae_db_realloc(): negative size", state);
    if( size<0 )
        return ae_false;
    
    /* realloc */
    if( block->ptr!=NULL )
        ((ae_deallocator)block->deallocator)(block->ptr);
    block->ptr = ae_malloc((size_t)size, state);
    if( block->ptr==NULL && size!=0 )
        return ae_false;
    block->deallocator = ae_free;
    return ae_true;
}


/************************************************************************
This function clears dynamic block (releases  all  dynamically  allocated
memory). Dynamic block may be in automatic management list - in this case
it will NOT be removed from list.

block               destination block (initialized)

NOTES:
* never call it for special blocks which marks frame boundaries!
************************************************************************/
void ae_db_free(ae_dyn_block *block)
{
    if( block->ptr!=NULL )
        ((ae_deallocator)block->deallocator)(block->ptr);
    block->ptr = NULL;
    block->deallocator = ae_free;
}

/************************************************************************
This function swaps contents of two dynamic blocks (pointers and 
deallocators) leaving other parameters (automatic management settings, 
etc.) unchanged.

NOTES:
* never call it for special blocks which marks frame boundaries!
************************************************************************/
void ae_db_swap(ae_dyn_block *block1, ae_dyn_block *block2)
{
    void (*deallocator)(void*) = NULL;
    void * volatile ptr;
    ptr = block1->ptr;
    deallocator = block1->deallocator;
    block1->ptr = block2->ptr;
    block1->deallocator = block2->deallocator;
    block2->ptr = ptr;
    block2->deallocator = deallocator;
}

/************************************************************************
This function creates ae_vector.

Vector size may be zero. Vector contents is uninitialized.

dst                 destination vector
size                vector size, may be zero
datatype            guess what...
state               ALGLIB environment state
make_automatic      if true, vector is added to the dynamic block list

Error handling:
* if state is NULL, returns ae_false on allocation error
* if state is not NULL, calls ae_break() on allocation error
* returns ae_true on success

dst is assumed to be uninitialized, its fields are ignored.
************************************************************************/
ae_bool ae_vector_init(ae_vector *dst, ae_int_t size, ae_datatype datatype, ae_state *state, ae_bool make_automatic)
{
    /* ensure that size is >=0
       two ways to exit: 1) through ae_assert, if we have non-NULL state, 2) by returning ae_false */
    if( state!=NULL )
        ae_assert(size>=0, "ae_vector_init(): negative size", state);
    if( size<0 )
        return ae_false;

    /* init */
    dst->cnt = size;
    dst->datatype = datatype;
    if( !ae_db_malloc(&dst->data, size*ae_sizeof(datatype), state, make_automatic) )
        return ae_false;
    dst->ptr.p_ptr = dst->data.ptr;
    return ae_true;
}


/************************************************************************
This function creates copy of ae_vector.

dst                 destination vector
src                 well, it is source
state               ALGLIB environment state
make_automatic      if true, vector is added to the dynamic block list

Error handling:
* if state is NULL, returns ae_false on allocation error
* if state is not NULL, calls ae_break() on allocation error
* returns ae_true on success

dst is assumed to be uninitialized, its fields are ignored.
************************************************************************/
ae_bool ae_vector_init_copy(ae_vector *dst, ae_vector *src, ae_state *state, ae_bool make_automatic)
{
    if( !ae_vector_init(dst, src->cnt, src->datatype, state, make_automatic) )
        return ae_false;
    if( src->cnt!=0 )
        memcpy(dst->ptr.p_ptr, src->ptr.p_ptr, (size_t)(src->cnt*ae_sizeof(src->datatype)));
    return ae_true;
}

/************************************************************************
This function creates ae_vector from x_vector:

dst                 destination vector
src                 source, vector in x-format
state               ALGLIB environment state
make_automatic      if true, vector is added to the dynamic block list

dst is assumed to be uninitialized, its fields are ignored.
************************************************************************/
void ae_vector_init_from_x(ae_vector *dst, x_vector *src, ae_state *state, ae_bool make_automatic)
{
    ae_vector_init(dst, (ae_int_t)src->cnt, (ae_datatype)src->datatype, state, make_automatic);
    if( src->cnt>0 )
        memcpy(dst->ptr.p_ptr, src->ptr, (size_t)(((ae_int_t)src->cnt)*ae_sizeof((ae_datatype)src->datatype)));
}


/************************************************************************
This function changes length of ae_vector.

dst                 destination vector
newsize             vector size, may be zero
state               ALGLIB environment state

Error handling:
* if state is NULL, returns ae_false on allocation error
* if state is not NULL, calls ae_break() on allocation error
* returns ae_true on success

NOTES:
* vector must be initialized
* all contents is destroyed during setlength() call
* new size may be zero.
************************************************************************/
ae_bool ae_vector_set_length(ae_vector *dst, ae_int_t newsize, ae_state *state)
{
    /* ensure that size is >=0
       two ways to exit: 1) through ae_assert, if we have non-NULL state, 2) by returning ae_false */
    if( state!=NULL )
        ae_assert(newsize>=0, "ae_vector_set_length(): negative size", state);
    if( newsize<0 )
        return ae_false;

    /* set length */
    if( dst->cnt==newsize )
        return ae_true;
    dst->cnt = newsize;
    if( !ae_db_realloc(&dst->data, newsize*ae_sizeof(dst->datatype), state) )
        return ae_false;
    dst->ptr.p_ptr = dst->data.ptr;
    return ae_true;
}


/************************************************************************
This function clears vector contents (releases all dynamically  allocated
memory). Vector may be in automatic management list  -  in this  case  it
will NOT be removed from list.

IMPORTANT: this function does NOT invalidates dst; it just  releases  all
dynamically allocated storage, but dst still may be used  after  call  to
ae_vector_set_length().

dst                 destination vector
************************************************************************/
void ae_vector_clear(ae_vector *dst)
{
    dst->cnt = 0;
    ae_db_free(&dst->data);
    dst->ptr.p_ptr = 0;
}


/************************************************************************
This function efficiently swaps contents of two vectors, leaving other
pararemeters (automatic management, etc.) unchanged.
************************************************************************/
void ae_swap_vectors(ae_vector *vec1, ae_vector *vec2)
{
    ae_int_t cnt;
    ae_datatype datatype;
    void *p_ptr;
    
    ae_db_swap(&vec1->data, &vec2->data);
    
    cnt = vec1->cnt;
    datatype = vec1->datatype;
    p_ptr = vec1->ptr.p_ptr;
    vec1->cnt = vec2->cnt;
    vec1->datatype = vec2->datatype;
    vec1->ptr.p_ptr = vec2->ptr.p_ptr;
    vec2->cnt = cnt;
    vec2->datatype = datatype;
    vec2->ptr.p_ptr = p_ptr;
}

/************************************************************************
This function creates ae_matrix.

Matrix size may be zero, in such cases both rows and cols are zero.
Matrix contents is uninitialized.

dst                 destination natrix
rows                rows count
cols                cols count
datatype            element type
state               ALGLIB environment state
make_automatic      if true, matrix is added to the dynamic block list

Error handling:
* if state is NULL, returns ae_false on allocation error
* if state is not NULL, calls ae_break() on allocation error
* returns ae_true on success

dst is assumed to be uninitialized, its fields are ignored.
************************************************************************/
ae_bool ae_matrix_init(ae_matrix *dst, ae_int_t rows, ae_int_t cols, ae_datatype datatype, ae_state *state, ae_bool make_automatic)
{
    /* ensure that size is >=0
       two ways to exit: 1) through ae_assert, if we have non-NULL state, 2) by returning ae_false */
    if( state!=NULL )
        ae_assert(rows>=0 && cols>=0, "ae_matrix_init(): negative length", state);
    if( rows<0 || cols<0 )
        return ae_false;

    /* if one of rows/cols is zero, another MUST be too */
    if( rows==0 || cols==0 )
    {
        rows = 0;
        cols = 0;
    }

    /* init */
    dst->rows = rows;
    dst->cols = cols;
    dst->stride = cols;
    while( dst->stride*ae_sizeof(datatype)%AE_DATA_ALIGN!=0 )
        dst->stride++;
    dst->datatype = datatype;
    if( !ae_db_malloc(&dst->data, dst->rows*((ae_int_t)sizeof(void*)+dst->stride*ae_sizeof(datatype))+AE_DATA_ALIGN-1, state, make_automatic) )
        return ae_false;
    ae_matrix_update_row_pointers(dst, ae_align((char*)dst->data.ptr+dst->rows*sizeof(void*),AE_DATA_ALIGN));
    return ae_true;
}


/************************************************************************
This function creates copy of ae_matrix.

dst                 destination matrix
src                 well, it is source
state               ALGLIB environment state
make_automatic      if true, matrix is added to the dynamic block list

Error handling:
* if state is NULL, returns ae_false on allocation error
* if state is not NULL, calls ae_break() on allocation error
* returns ae_true on success

dst is assumed to be uninitialized, its fields are ignored.
************************************************************************/
ae_bool ae_matrix_init_copy(ae_matrix *dst, ae_matrix *src, ae_state *state, ae_bool make_automatic)
{
    ae_int_t i;
    if( !ae_matrix_init(dst, src->rows, src->cols, src->datatype, state, make_automatic) )
        return ae_false;
    if( src->rows!=0 && src->cols!=0 )
    {
        if( dst->stride==src->stride )
            memcpy(dst->ptr.pp_void[0], src->ptr.pp_void[0], (size_t)(src->rows*src->stride*ae_sizeof(src->datatype)));
        else
            for(i=0; i<dst->rows; i++)
                memcpy(dst->ptr.pp_void[i], src->ptr.pp_void[i], (size_t)(dst->cols*ae_sizeof(dst->datatype)));
    }
    return ae_true;
}


void ae_matrix_init_from_x(ae_matrix *dst, x_matrix *src, ae_state *state, ae_bool make_automatic)
{
    char *p_src_row;
    char *p_dst_row;
    ae_int_t row_size;
    ae_int_t i;
    ae_matrix_init(dst, (ae_int_t)src->rows, (ae_int_t)src->cols, (ae_datatype)src->datatype, state, make_automatic);
    if( src->rows!=0 && src->cols!=0 )
    {
        p_src_row = (char*)src->ptr;
        p_dst_row = (char*)(dst->ptr.pp_void[0]);
        row_size = ae_sizeof((ae_datatype)src->datatype)*(ae_int_t)src->cols;
        for(i=0; i<src->rows; i++, p_src_row+=src->stride*ae_sizeof((ae_datatype)src->datatype), p_dst_row+=dst->stride*ae_sizeof((ae_datatype)src->datatype))
            memcpy(p_dst_row, p_src_row, (size_t)(row_size));
    }
}


/************************************************************************
This function changes length of ae_matrix.

dst                 destination matrix
rows                size, may be zero
cols                size, may be zero
state               ALGLIB environment state

Error handling:
* if state is NULL, returns ae_false on allocation error
* if state is not NULL, calls ae_break() on allocation error
* returns ae_true on success

NOTES:
* matrix must be initialized
* all contents is destroyed during setlength() call
* new size may be zero.
************************************************************************/
ae_bool ae_matrix_set_length(ae_matrix *dst, ae_int_t rows, ae_int_t cols, ae_state *state)
{
    /* ensure that size is >=0
       two ways to exit: 1) through ae_assert, if we have non-NULL state, 2) by returning ae_false */
    if( state!=NULL )
        ae_assert(rows>=0 && cols>=0, "ae_matrix_set_length(): negative length", state);
    if( rows<0 || cols<0 )
        return ae_false;

    if( dst->rows==rows && dst->cols==cols )
        return ae_true;    
    dst->rows = rows;
    dst->cols = cols;
    dst->stride = cols;
    while( dst->stride*ae_sizeof(dst->datatype)%AE_DATA_ALIGN!=0 )
        dst->stride++;
    if( !ae_db_realloc(&dst->data, dst->rows*((ae_int_t)sizeof(void*)+dst->stride*ae_sizeof(dst->datatype))+AE_DATA_ALIGN-1, state) )
        return ae_false;
    ae_matrix_update_row_pointers(dst, ae_align((char*)dst->data.ptr+dst->rows*sizeof(void*),AE_DATA_ALIGN));
    return ae_true;
}


/************************************************************************
This function clears matrix contents (releases all dynamically  allocated
memory). Matrix may be in automatic management list  -  in this  case  it
will NOT be removed from list.

IMPORTANT: this function does NOT invalidates dst; it just  releases  all
dynamically allocated storage, but dst still may be used  after  call  to
ae_matrix_set_length().

dst                 destination matrix
************************************************************************/
void ae_matrix_clear(ae_matrix *dst)
{
    dst->rows = 0;
    dst->cols = 0;
    dst->stride = 0;
    ae_db_free(&dst->data);
    dst->ptr.p_ptr = 0;
}


/************************************************************************
This function efficiently swaps contents of two vectors, leaving other
pararemeters (automatic management, etc.) unchanged.
************************************************************************/
void ae_swap_matrices(ae_matrix *mat1, ae_matrix *mat2)
{
    ae_int_t rows;
    ae_int_t cols;
    ae_int_t stride;
    ae_datatype datatype;
    void *p_ptr;
    
    ae_db_swap(&mat1->data, &mat2->data);
    
    rows = mat1->rows;
    cols = mat1->cols;
    stride = mat1->stride;
    datatype = mat1->datatype;
    p_ptr = mat1->ptr.p_ptr;

    mat1->rows = mat2->rows;
    mat1->cols = mat2->cols;
    mat1->stride = mat2->stride;
    mat1->datatype = mat2->datatype;
    mat1->ptr.p_ptr = mat2->ptr.p_ptr;

    mat2->rows = rows;
    mat2->cols = cols;
    mat2->stride = stride;
    mat2->datatype = datatype;
    mat2->ptr.p_ptr = p_ptr;
}

/************************************************************************
This function fills x_vector by ae_vector's contents:

dst                 destination vector
src                 source, vector in x-format
state               ALGLIB environment state

NOTES:
* dst is assumed to be initialized. Its contents is freed before  copying
  data  from src  (if  size / type  are  different)  or  overwritten  (if
  possible given destination size).
************************************************************************/
void ae_x_set_vector(x_vector *dst, ae_vector *src, ae_state *state)
{
    if( dst->cnt!=src->cnt || dst->datatype!=src->datatype )
    {
        if( dst->owner==OWN_AE )
            ae_free(dst->ptr);
        dst->ptr = ae_malloc((size_t)(src->cnt*ae_sizeof(src->datatype)), state);
        dst->last_action = ACT_NEW_LOCATION;
        dst->cnt = src->cnt;
        dst->datatype = src->datatype;
        dst->owner = OWN_AE;
    }
    else
        dst->last_action = ACT_SAME_LOCATION;
    if( src->cnt )
        memcpy(dst->ptr, src->ptr.p_ptr, (size_t)(src->cnt*ae_sizeof(src->datatype)));
}

/************************************************************************
This function fills x_matrix by ae_matrix's contents:

dst                 destination vector
src                 source, matrix in x-format
state               ALGLIB environment state

NOTES:
* dst is assumed to be initialized. Its contents is freed before  copying
  data  from src  (if  size / type  are  different)  or  overwritten  (if
  possible given destination size).
************************************************************************/
void ae_x_set_matrix(x_matrix *dst, ae_matrix *src, ae_state *state)
{
    char *p_src_row;
    char *p_dst_row;
    ae_int_t i;
    ae_int_t row_size;
    if( dst->rows!=src->rows || dst->cols!=src->cols || dst->datatype!=src->datatype )
    {
        if( dst->owner==OWN_AE )
            ae_free(dst->ptr);
        dst->rows = src->rows;
        dst->cols = src->cols;
        dst->stride = src->cols;
        dst->datatype = src->datatype;
        dst->ptr = ae_malloc((size_t)(dst->rows*((ae_int_t)dst->stride)*ae_sizeof(src->datatype)), state);
        dst->last_action = ACT_NEW_LOCATION;
        dst->owner = OWN_AE;
    }
    else
        dst->last_action = ACT_SAME_LOCATION;
    if( src->rows!=0 && src->cols!=0 )
    {
        p_src_row = (char*)(src->ptr.pp_void[0]);
        p_dst_row = (char*)dst->ptr;
        row_size = ae_sizeof(src->datatype)*src->cols;
        for(i=0; i<src->rows; i++, p_src_row+=src->stride*ae_sizeof(src->datatype), p_dst_row+=dst->stride*ae_sizeof(src->datatype))
            memcpy(p_dst_row, p_src_row, (size_t)(row_size));
    }
}

/************************************************************************
This function attaches x_vector to ae_vector's contents.
Ownership of memory allocated is not changed (it is still managed by
ae_matrix).

dst                 destination vector
src                 source, vector in x-format
state               ALGLIB environment state

NOTES:
* dst is assumed to be initialized. Its contents is freed before
  attaching to src.
* this function doesn't need ae_state parameter because it can't fail
  (assuming correctly initialized src)
************************************************************************/
void ae_x_attach_to_vector(x_vector *dst, ae_vector *src)
{
    if( dst->owner==OWN_AE )
        ae_free(dst->ptr);
    dst->ptr = src->ptr.p_ptr;
    dst->last_action = ACT_NEW_LOCATION;
    dst->cnt = src->cnt;
    dst->datatype = src->datatype;
    dst->owner = OWN_CALLER;
}

/************************************************************************
This function attaches x_matrix to ae_matrix's contents.
Ownership of memory allocated is not changed (it is still managed by
ae_matrix).

dst                 destination vector
src                 source, matrix in x-format
state               ALGLIB environment state

NOTES:
* dst is assumed to be initialized. Its contents is freed before
  attaching to src.
* this function doesn't need ae_state parameter because it can't fail
  (assuming correctly initialized src)
************************************************************************/
void ae_x_attach_to_matrix(x_matrix *dst, ae_matrix *src)
{
    if( dst->owner==OWN_AE )
            ae_free(dst->ptr);
    dst->rows = src->rows;
    dst->cols = src->cols;
    dst->stride = src->stride;
    dst->datatype = src->datatype;
    dst->ptr = &(src->ptr.pp_double[0][0]);
    dst->last_action = ACT_NEW_LOCATION;
    dst->owner = OWN_CALLER;
}

/************************************************************************
This function clears x_vector. It does nothing  if vector is not owned by
ALGLIB environment.

dst                 vector
************************************************************************/
void x_vector_clear(x_vector *dst)
{
    if( dst->owner==OWN_AE )
        aligned_free(dst->ptr);
    dst->ptr = NULL;
    dst->cnt = 0;
}

/************************************************************************
Service functions
************************************************************************/
void ae_assert(ae_bool cond, const char *msg, ae_state *state)
{
    if( !cond )
        ae_break(state, ERR_ASSERTION_FAILED, msg);
}

/************************************************************************
Real math functions
************************************************************************/
ae_bool ae_fp_eq(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    volatile double x = v1;
    volatile double y = v2;
    return x==y;
}

ae_bool ae_fp_neq(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    return !ae_fp_eq(v1,v2);
}

ae_bool ae_fp_less(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    volatile double x = v1;
    volatile double y = v2;
    return x<y;
}

ae_bool ae_fp_less_eq(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    volatile double x = v1;
    volatile double y = v2;
    return x<=y;
}

ae_bool ae_fp_greater(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    volatile double x = v1;
    volatile double y = v2;
    return x>y;
}

ae_bool ae_fp_greater_eq(double v1, double v2)
{
    /* IEEE-strict floating point comparison */
    volatile double x = v1;
    volatile double y = v2;
    return x>=y;
}

ae_bool ae_isfinite_stateless(double x, ae_int_t endianness)
{
    union _u
    {
        double a;
        ae_int32_t p[2];
    } u;
    ae_int32_t high;
    u.a = x;
    if( endianness==AE_LITTLE_ENDIAN )
        high = u.p[1];
    else
        high = u.p[0];
    return (high & (ae_int32_t)0x7FF00000)!=(ae_int32_t)0x7FF00000;
}

ae_bool ae_isnan_stateless(double x,    ae_int_t endianness)
{
    union _u
    {
        double a;
        ae_int32_t p[2];
    } u;
    ae_int32_t high, low;
    u.a = x;
    if( endianness==AE_LITTLE_ENDIAN )
    {
        high = u.p[1];
        low =  u.p[0];
    }
    else
    {
        high = u.p[0];
        low =  u.p[1];
    }
    return ((high &0x7FF00000)==0x7FF00000) && (((high &0x000FFFFF)!=0) || (low!=0));
}

ae_bool ae_isinf_stateless(double x,    ae_int_t endianness)
{
    union _u
    {
        double a;
        ae_int32_t p[2];
    } u;
    ae_int32_t high, low;
    u.a = x;
    if( endianness==AE_LITTLE_ENDIAN )
    {
        high = u.p[1];
        low  = u.p[0];
    }
    else
    {
        high = u.p[0];
        low  = u.p[1];
    }
    
    // 31 least significant bits of high are compared
    return ((high&0x7FFFFFFF)==0x7FF00000) && (low==0); 
}

ae_bool ae_isposinf_stateless(double x, ae_int_t endianness)
{
    union _u
    {
        double a;
        ae_int32_t p[2];
    } u;
    ae_int32_t high, low;
    u.a = x;
    if( endianness==AE_LITTLE_ENDIAN )
    {
        high = u.p[1];
        low  = u.p[0];
    }
    else
    {
        high = u.p[0];
        low  = u.p[1];
    }
    
    // all 32 bits of high are compared
    return (high==(ae_int32_t)0x7FF00000) && (low==0); 
}

ae_bool ae_isneginf_stateless(double x, ae_int_t endianness)
{
    union _u
    {
        double a;
        ae_int32_t p[2];
    } u;
    ae_int32_t high, low;
    u.a = x;
    if( endianness==AE_LITTLE_ENDIAN )
    {
        high = u.p[1];
        low  = u.p[0];
    }
    else
    {
        high = u.p[0];
        low  = u.p[1];
    }
    
    // this code is a bit tricky to avoid comparison of high with 0xFFF00000, which may be unsafe with some buggy compilers
    return ((high&0x7FFFFFFF)==0x7FF00000) && (high!=(ae_int32_t)0x7FF00000) && (low==0);
}

ae_int_t ae_get_endianness()
{
    union
    {
        double a;
        ae_int32_t p[2];
    } u;
    
    /*
     * determine endianness
     * two types are supported: big-endian and little-endian.
     * mixed-endian hardware is NOT supported.
     *
     * 1983 is used as magic number because its non-periodic double 
     * representation allow us to easily distinguish between upper 
     * and lower halfs and to detect mixed endian hardware.
     *
     */
    u.a = 1.0/1983.0; 
    if( u.p[1]==(ae_int32_t)0x3f408642 )
        return AE_LITTLE_ENDIAN;
    if( u.p[0]==(ae_int32_t)0x3f408642 )
        return AE_BIG_ENDIAN;
    return AE_MIXED_ENDIAN;
}

ae_bool ae_isfinite(double x,ae_state *state)
{
    return ae_isfinite_stateless(x, state->endianness);
}

ae_bool ae_isnan(double x,   ae_state *state)
{
    return ae_isnan_stateless(x, state->endianness);
}

ae_bool ae_isinf(double x,   ae_state *state)
{
    return ae_isinf_stateless(x, state->endianness);
}

ae_bool ae_isposinf(double x,ae_state *state)
{
    return ae_isposinf_stateless(x, state->endianness);
}

ae_bool ae_isneginf(double x,ae_state *state)
{
    return ae_isneginf_stateless(x, state->endianness);
}

double ae_fabs(double x,  ae_state *state)
{
    return fabs(x);
}

ae_int_t ae_iabs(ae_int_t x, ae_state *state)
{
    return x>=0 ? x : -x;
}

double ae_sqr(double x,  ae_state *state)
{
    return x*x;
}

double ae_sqrt(double x, ae_state *state)
{
    return sqrt(x);
}

ae_int_t ae_sign(double x, ae_state *state)
{
    if( x>0 ) return  1;
    if( x<0 ) return -1;
    return 0;
}

ae_int_t ae_round(double x, ae_state *state)
{
    return (ae_int_t)(ae_ifloor(x+0.5,state));
}

ae_int_t ae_trunc(double x, ae_state *state)
{
    return (ae_int_t)(x>0 ? ae_ifloor(x,state) : ae_iceil(x,state));
}

ae_int_t ae_ifloor(double x, ae_state *state)
{
    return (ae_int_t)(floor(x));
}

ae_int_t ae_iceil(double x,  ae_state *state)
{
    return (ae_int_t)(ceil(x));
}

ae_int_t ae_maxint(ae_int_t m1, ae_int_t m2, ae_state *state)
{
    return m1>m2 ? m1 : m2;
}

ae_int_t ae_minint(ae_int_t m1, ae_int_t m2, ae_state *state)
{
    return m1>m2 ? m2 : m1;
}

double ae_maxreal(double m1, double m2, ae_state *state)
{
    return m1>m2 ? m1 : m2;
}

double ae_minreal(double m1, double m2, ae_state *state)
{
    return m1>m2 ? m2 : m1;
}

double ae_randomreal(ae_state *state)
{
    int i1 = rand();
    int i2 = rand();
    double mx;
    while(i1==RAND_MAX)
        i1 =rand();
    while(i2==RAND_MAX)
        i2 =rand();
    mx = RAND_MAX;
    return (i1+i2/mx)/mx;
}

ae_int_t ae_randominteger(ae_int_t maxv, ae_state *state)
{
    return rand()%maxv;
}

double   ae_sin(double x, ae_state *state)
{
    return sin(x);
}

double   ae_cos(double x, ae_state *state)
{
    return cos(x);
}

double   ae_tan(double x, ae_state *state)
{
    return tan(x);
}

double   ae_sinh(double x, ae_state *state)
{
    return sinh(x);
}

double   ae_cosh(double x, ae_state *state)
{
    return cosh(x);
}
double   ae_tanh(double x, ae_state *state)
{
    return tanh(x);
}

double   ae_asin(double x, ae_state *state)
{
    return asin(x);
}

double   ae_acos(double x, ae_state *state)
{
    return acos(x);
}

double   ae_atan(double x, ae_state *state)
{
    return atan(x);
}

double   ae_atan2(double x, double y, ae_state *state)
{
    return atan2(x,y);
}

double   ae_log(double x, ae_state *state)
{
    return log(x);
}

double   ae_pow(double x, double y, ae_state *state)
{
    return pow(x,y);
}

double   ae_exp(double x, ae_state *state)
{
    return exp(x);
}

/************************************************************************
Symmetric/Hermitian properties: check and force
************************************************************************/
static void x_split_length(ae_int_t n, ae_int_t nb, ae_int_t* n1, ae_int_t* n2)
{
    ae_int_t r;
    if( n<=nb )
    {
        *n1 = n;
        *n2 = 0;
    }
    else
    {
        if( n%nb!=0 )
        {
            *n2 = n%nb;
            *n1 = n-(*n2);
        }
        else
        {
            *n2 = n/2;
            *n1 = n-(*n2);
            if( *n1%nb==0 )
            {
                return;
            }
            r = nb-*n1%nb;
            *n1 = *n1+r;
            *n2 = *n2-r;
        }
    }
}
static double x_safepythag2(double x, double y)
{
    double w;
    double xabs;
    double yabs;
    double z;
    xabs = fabs(x);
    yabs = fabs(y);
    w = xabs>yabs ? xabs : yabs;
    z = xabs<yabs ? xabs : yabs;
    if( z==0 )
        return w;
    else
    {
        double t;
        t = z/w;
        return w*sqrt(1+t*t);
    }
}
/*
 * this function checks difference between offdiagonal blocks BL and BU
 * (see below). Block BL is specified by offsets (offset0,offset1)  and
 * sizes (len0,len1).
 *
 *     [ .          ]
 *     [   A0  BU   ]
 * A = [   BL  A1   ]
 *     [          . ]
 *
 *  this subroutine updates current values of:
 *  a) mx       maximum value of A[i,j] found so far
 *  b) err      componentwise difference between elements of BL and BU^T
 *
 */
static void is_symmetric_rec_off_stat(x_matrix *a, ae_int_t offset0, ae_int_t offset1, ae_int_t len0, ae_int_t len1, ae_bool *nonfinite, double *mx, double *err, ae_state *_state)
{
    /* try to split problem into two smaller ones */
    if( len0>x_nb || len1>x_nb )
    {
        ae_int_t n1, n2;
        if( len0>len1 )
        {
            x_split_length(len0, x_nb, &n1, &n2);
            is_symmetric_rec_off_stat(a, offset0,    offset1, n1, len1, nonfinite, mx, err, _state);
            is_symmetric_rec_off_stat(a, offset0+n1, offset1, n2, len1, nonfinite, mx, err, _state);
        }
        else
        {
            x_split_length(len1, x_nb, &n1, &n2);
            is_symmetric_rec_off_stat(a, offset0, offset1,    len0, n1, nonfinite, mx, err, _state);
            is_symmetric_rec_off_stat(a, offset0, offset1+n1, len0, n2, nonfinite, mx, err, _state);
        }
        return;
    }
    else
    {
        /* base case */
        double *p1, *p2, *prow, *pcol;
        double v;
        ae_int_t i, j;

        p1 = (double*)(a->ptr)+offset0*a->stride+offset1;
        p2 = (double*)(a->ptr)+offset1*a->stride+offset0;
        for(i=0; i<len0; i++)
        {
            pcol = p2+i;
            prow = p1+i*a->stride;
            for(j=0; j<len1; j++)
            {
                if( !ae_isfinite(*pcol,_state) || !ae_isfinite(*prow,_state) )
                {
                    *nonfinite = ae_true;
                }
                else
                {
                    v = fabs(*pcol);
                    *mx =  *mx>v ? *mx : v;
                    v = fabs(*prow);
                    *mx =  *mx>v ? *mx : v;
                    v = fabs(*pcol-*prow);
                    *err = *err>v ? *err : v;
                }                
                pcol += a->stride;
                prow++;
            }
        }
    }
}
/*
 * this function checks that diagonal block A0 is symmetric.
 * Block A0 is specified by its offset and size.
 *
 *     [ .          ]
 *     [   A0       ]
 * A = [       .    ]
 *     [          . ]
 *
 *  this subroutine updates current values of:
 *  a) mx       maximum value of A[i,j] found so far
 *  b) err      componentwise difference between A0 and A0^T
 *
 */
static void is_symmetric_rec_diag_stat(x_matrix *a, ae_int_t offset, ae_int_t len, ae_bool *nonfinite, double *mx, double *err, ae_state *_state)
{
    double *p, *prow, *pcol;
    double v;
    ae_int_t i, j;
    
    /* try to split problem into two smaller ones */
    if( len>x_nb )
    {
        ae_int_t n1, n2;
        x_split_length(len, x_nb, &n1, &n2);
        is_symmetric_rec_diag_stat(a, offset, n1, nonfinite, mx, err, _state);
        is_symmetric_rec_diag_stat(a, offset+n1, n2, nonfinite, mx, err, _state);
        is_symmetric_rec_off_stat(a, offset+n1, offset, n2, n1, nonfinite, mx, err, _state);
        return;
    }
    
    /* base case */
    p = (double*)(a->ptr)+offset*a->stride+offset;
    for(i=0; i<len; i++)
    {
        pcol = p+i;
        prow = p+i*a->stride;
        for(j=0; j<i; j++,pcol+=a->stride,prow++)
        {
            if( !ae_isfinite(*pcol,_state) || !ae_isfinite(*prow,_state) )
            {
                *nonfinite = ae_true;
            }
            else
            {
                v = fabs(*pcol);
                *mx =  *mx>v ? *mx : v;
                v = fabs(*prow);
                *mx =  *mx>v ? *mx : v;
                v = fabs(*pcol-*prow);
                *err = *err>v ? *err : v;
            }
        }
        v = fabs(p[i+i*a->stride]);
        *mx =  *mx>v ? *mx : v;
    }
}
/*
 * this function checks difference between offdiagonal blocks BL and BU
 * (see below). Block BL is specified by offsets (offset0,offset1)  and
 * sizes (len0,len1).
 *
 *     [ .          ]
 *     [   A0  BU   ]
 * A = [   BL  A1   ]
 *     [          . ]
 *
 *  this subroutine updates current values of:
 *  a) mx       maximum value of A[i,j] found so far
 *  b) err      componentwise difference between elements of BL and BU^H
 *
 */
static void is_hermitian_rec_off_stat(x_matrix *a, ae_int_t offset0, ae_int_t offset1, ae_int_t len0, ae_int_t len1, ae_bool *nonfinite, double *mx, double *err, ae_state *_state)
{
    /* try to split problem into two smaller ones */
    if( len0>x_nb || len1>x_nb )
    {
        ae_int_t n1, n2;
        if( len0>len1 )
        {
            x_split_length(len0, x_nb, &n1, &n2);
            is_hermitian_rec_off_stat(a, offset0,    offset1, n1, len1, nonfinite, mx, err, _state);
            is_hermitian_rec_off_stat(a, offset0+n1, offset1, n2, len1, nonfinite, mx, err, _state);
        }
        else
        {
            x_split_length(len1, x_nb, &n1, &n2);
            is_hermitian_rec_off_stat(a, offset0, offset1,    len0, n1, nonfinite, mx, err, _state);
            is_hermitian_rec_off_stat(a, offset0, offset1+n1, len0, n2, nonfinite, mx, err, _state);
        }
        return;
    }
    else
    {
        /* base case */
        ae_complex *p1, *p2, *prow, *pcol;
        double v;
        ae_int_t i, j;

        p1 = (ae_complex*)(a->ptr)+offset0*a->stride+offset1;
        p2 = (ae_complex*)(a->ptr)+offset1*a->stride+offset0;
        for(i=0; i<len0; i++)
        {
            pcol = p2+i;
            prow = p1+i*a->stride;
            for(j=0; j<len1; j++)
            {
                if( !ae_isfinite(pcol->x, _state) || !ae_isfinite(pcol->y, _state) || !ae_isfinite(prow->x, _state) || !ae_isfinite(prow->y, _state) )
                {
                    *nonfinite = ae_true;
                }
                else
                {
                    v = x_safepythag2(pcol->x, pcol->y);
                    *mx =  *mx>v ? *mx : v;
                    v = x_safepythag2(prow->x, prow->y);
                    *mx =  *mx>v ? *mx : v;
                    v = x_safepythag2(pcol->x-prow->x, pcol->y+prow->y);
                    *err = *err>v ? *err : v;
                }
                pcol += a->stride;
                prow++;
            }
        }
    }
}
/*
 * this function checks that diagonal block A0 is Hermitian.
 * Block A0 is specified by its offset and size.
 *
 *     [ .          ]
 *     [   A0       ]
 * A = [       .    ]
 *     [          . ]
 *
 *  this subroutine updates current values of:
 *  a) mx       maximum value of A[i,j] found so far
 *  b) err      componentwise difference between A0 and A0^H
 *
 */
static void is_hermitian_rec_diag_stat(x_matrix *a, ae_int_t offset, ae_int_t len, ae_bool *nonfinite, double *mx, double *err, ae_state *_state)
{
    ae_complex *p, *prow, *pcol;
    double v;
    ae_int_t i, j;
    
    /* try to split problem into two smaller ones */
    if( len>x_nb )
    {
        ae_int_t n1, n2;
        x_split_length(len, x_nb, &n1, &n2);
        is_hermitian_rec_diag_stat(a, offset, n1, nonfinite, mx, err, _state);
        is_hermitian_rec_diag_stat(a, offset+n1, n2, nonfinite, mx, err, _state);
        is_hermitian_rec_off_stat(a, offset+n1, offset, n2, n1, nonfinite, mx, err, _state);
        return;
    }
    
    /* base case */
    p = (ae_complex*)(a->ptr)+offset*a->stride+offset;
    for(i=0; i<len; i++)
    {
        pcol = p+i;
        prow = p+i*a->stride;
        for(j=0; j<i; j++,pcol+=a->stride,prow++)
        {
            if( !ae_isfinite(pcol->x, _state) || !ae_isfinite(pcol->y, _state) || !ae_isfinite(prow->x, _state) || !ae_isfinite(prow->y, _state) )
            {
                *nonfinite = ae_true;
            }
            else
            {
                v = x_safepythag2(pcol->x, pcol->y);
                *mx =  *mx>v ? *mx : v;
                v = x_safepythag2(prow->x, prow->y);
                *mx =  *mx>v ? *mx : v;
                v = x_safepythag2(pcol->x-prow->x, pcol->y+prow->y);
                *err = *err>v ? *err : v;
            }
        }
        if( !ae_isfinite(p[i+i*a->stride].x, _state) || !ae_isfinite(p[i+i*a->stride].y, _state) )
        {
            *nonfinite = ae_true;
        }
        else
        {
            v = fabs(p[i+i*a->stride].x);
            *mx =  *mx>v ? *mx : v;
            v = fabs(p[i+i*a->stride].y);
            *err =  *err>v ? *err : v;
        }
    }
}
/*
 * this function copies offdiagonal block BL to its symmetric counterpart
 * BU (see below). Block BL is specified by offsets (offset0,offset1)
 * and sizes (len0,len1).
 *
 *     [ .          ]
 *     [   A0  BU   ]
 * A = [   BL  A1   ]
 *     [          . ]
 *
 */
static void force_symmetric_rec_off_stat(x_matrix *a, ae_int_t offset0, ae_int_t offset1, ae_int_t len0, ae_int_t len1)
{
    /* try to split problem into two smaller ones */
    if( len0>x_nb || len1>x_nb )
    {
        ae_int_t n1, n2;
        if( len0>len1 )
        {
            x_split_length(len0, x_nb, &n1, &n2);
            force_symmetric_rec_off_stat(a, offset0,    offset1, n1, len1);
            force_symmetric_rec_off_stat(a, offset0+n1, offset1, n2, len1);
        }
        else
        {
            x_split_length(len1, x_nb, &n1, &n2);
            force_symmetric_rec_off_stat(a, offset0, offset1,    len0, n1);
            force_symmetric_rec_off_stat(a, offset0, offset1+n1, len0, n2);
        }
        return;
    }
    else
    {
        /* base case */
        double *p1, *p2, *prow, *pcol;
        ae_int_t i, j;

        p1 = (double*)(a->ptr)+offset0*a->stride+offset1;
        p2 = (double*)(a->ptr)+offset1*a->stride+offset0;
        for(i=0; i<len0; i++)
        {
            pcol = p2+i;
            prow = p1+i*a->stride;
            for(j=0; j<len1; j++)
            {
                *pcol = *prow;
                pcol += a->stride;
                prow++;
            }
        }
    }
}
/*
 * this function copies lower part of diagonal block A0 to its upper part
 * Block is specified by offset and size.
 *
 *     [ .          ]
 *     [   A0       ]
 * A = [       .    ]
 *     [          . ]
 *
 */
static void force_symmetric_rec_diag_stat(x_matrix *a, ae_int_t offset, ae_int_t len)
{
    double *p, *prow, *pcol;
    ae_int_t i, j;
    
    /* try to split problem into two smaller ones */
    if( len>x_nb )
    {
        ae_int_t n1, n2;
        x_split_length(len, x_nb, &n1, &n2);
        force_symmetric_rec_diag_stat(a, offset, n1);
        force_symmetric_rec_diag_stat(a, offset+n1, n2);
        force_symmetric_rec_off_stat(a, offset+n1, offset, n2, n1);
        return;
    }
    
    /* base case */
    p = (double*)(a->ptr)+offset*a->stride+offset;
    for(i=0; i<len; i++)
    {
        pcol = p+i;
        prow = p+i*a->stride;
        for(j=0; j<i; j++,pcol+=a->stride,prow++)
            *pcol = *prow;
    }
}
/*
 * this function copies Hermitian transpose of offdiagonal block BL to
 * its symmetric counterpart BU (see below). Block BL is specified by
 * offsets (offset0,offset1) and sizes (len0,len1).
 *
 *     [ .          ]
 *     [   A0  BU   ]
 * A = [   BL  A1   ]
 *     [          . ]
 */
static void force_hermitian_rec_off_stat(x_matrix *a, ae_int_t offset0, ae_int_t offset1, ae_int_t len0, ae_int_t len1)
{
    /* try to split problem into two smaller ones */
    if( len0>x_nb || len1>x_nb )
    {
        ae_int_t n1, n2;
        if( len0>len1 )
        {
            x_split_length(len0, x_nb, &n1, &n2);
            force_hermitian_rec_off_stat(a, offset0,    offset1, n1, len1);
            force_hermitian_rec_off_stat(a, offset0+n1, offset1, n2, len1);
        }
        else
        {
            x_split_length(len1, x_nb, &n1, &n2);
            force_hermitian_rec_off_stat(a, offset0, offset1,    len0, n1);
            force_hermitian_rec_off_stat(a, offset0, offset1+n1, len0, n2);
        }
        return;
    }
    else
    {
        /* base case */
        ae_complex *p1, *p2, *prow, *pcol;
        ae_int_t i, j;

        p1 = (ae_complex*)(a->ptr)+offset0*a->stride+offset1;
        p2 = (ae_complex*)(a->ptr)+offset1*a->stride+offset0;
        for(i=0; i<len0; i++)
        {
            pcol = p2+i;
            prow = p1+i*a->stride;
            for(j=0; j<len1; j++)
            {
                *pcol = *prow;
                pcol += a->stride;
                prow++;
            }
        }
    }
}
/*
 * this function copies Hermitian transpose of lower part of
 * diagonal block A0 to its upper part Block is specified by offset and size.
 *
 *     [ .          ]
 *     [   A0       ]
 * A = [       .    ]
 *     [          . ]
 *
 */
static void force_hermitian_rec_diag_stat(x_matrix *a, ae_int_t offset, ae_int_t len)
{
    ae_complex *p, *prow, *pcol;
    ae_int_t i, j;
    
    /* try to split problem into two smaller ones */
    if( len>x_nb )
    {
        ae_int_t n1, n2;
        x_split_length(len, x_nb, &n1, &n2);
        force_hermitian_rec_diag_stat(a, offset, n1);
        force_hermitian_rec_diag_stat(a, offset+n1, n2);
        force_hermitian_rec_off_stat(a, offset+n1, offset, n2, n1);
        return;
    }
    
    /* base case */
    p = (ae_complex*)(a->ptr)+offset*a->stride+offset;
    for(i=0; i<len; i++)
    {
        pcol = p+i;
        prow = p+i*a->stride;
        for(j=0; j<i; j++,pcol+=a->stride,prow++)
            *pcol = *prow;
    }
}
ae_bool x_is_symmetric(x_matrix *a)
{
    double mx, err;
    ae_bool nonfinite;
    ae_state _alglib_env_state;
    if( a->datatype!=DT_REAL )
        return ae_false;
    if( a->cols!=a->rows )
        return ae_false;
    if( a->cols==0 || a->rows==0 )
        return ae_true;
    ae_state_init(&_alglib_env_state);
    mx = 0;
    err = 0;
    nonfinite = ae_false;
    is_symmetric_rec_diag_stat(a, 0, (ae_int_t)a->rows, &nonfinite, &mx, &err, &_alglib_env_state);
    if( nonfinite )
        return ae_false;
    if( mx==0 )
        return ae_true;
    return err/mx<=1.0E-14;
}
ae_bool x_is_hermitian(x_matrix *a)
{
    double mx, err;
    ae_bool nonfinite;
    ae_state _alglib_env_state;
    if( a->datatype!=DT_COMPLEX )
        return ae_false;
    if( a->cols!=a->rows )
        return ae_false;
    if( a->cols==0 || a->rows==0 )
        return ae_true;
    ae_state_init(&_alglib_env_state);
    mx = 0;
    err = 0;
    nonfinite = ae_false;
    is_hermitian_rec_diag_stat(a, 0, (ae_int_t)a->rows, &nonfinite, &mx, &err, &_alglib_env_state);
    if( nonfinite )
        return ae_false;
    if( mx==0 )
        return ae_true;
    return err/mx<=1.0E-14;
}
ae_bool x_force_symmetric(x_matrix *a)
{
    if( a->datatype!=DT_REAL )
        return ae_false;
    if( a->cols!=a->rows )
        return ae_false;
    if( a->cols==0 || a->rows==0 )
        return ae_true;
    force_symmetric_rec_diag_stat(a, 0, (ae_int_t)a->rows);
    return ae_true;
}
ae_bool x_force_hermitian(x_matrix *a)
{
    if( a->datatype!=DT_COMPLEX )
        return ae_false;
    if( a->cols!=a->rows )
        return ae_false;
    if( a->cols==0 || a->rows==0 )
        return ae_true;
    force_hermitian_rec_diag_stat(a, 0, (ae_int_t)a->rows);
    return ae_true;
}

ae_bool ae_is_symmetric(ae_matrix *a)
{
    x_matrix x;
    x.owner = OWN_CALLER;
    ae_x_attach_to_matrix(&x, a);
    return x_is_symmetric(&x);
}

ae_bool ae_is_hermitian(ae_matrix *a)
{
    x_matrix x;
    x.owner = OWN_CALLER;
    ae_x_attach_to_matrix(&x, a);
    return x_is_hermitian(&x);
}

ae_bool ae_force_symmetric(ae_matrix *a)
{
    x_matrix x;
    x.owner = OWN_CALLER;
    ae_x_attach_to_matrix(&x, a);
    return x_force_symmetric(&x);
}

ae_bool ae_force_hermitian(ae_matrix *a)
{
    x_matrix x;
    x.owner = OWN_CALLER;
    ae_x_attach_to_matrix(&x, a);
    return x_force_hermitian(&x);
}


/************************************************************************
Complex math functions
************************************************************************/
ae_complex ae_complex_from_d(double v)
{
    ae_complex r;
    r.x = v;
    r.y = 0.0;
    return r;
}

ae_complex ae_c_neg(ae_complex lhs)
{
    ae_complex result;
    result.x = -lhs.x;
    result.y = -lhs.y;
    return result;
}

ae_complex ae_c_conj(ae_complex lhs, ae_state *state)
{
    ae_complex result;
    result.x = +lhs.x;
    result.y = -lhs.y;
    return result;
}

ae_complex ae_c_sqr(ae_complex lhs, ae_state *state)
{
    ae_complex result;
    result.x = lhs.x*lhs.x-lhs.y*lhs.y;
    result.y = 2*lhs.x*lhs.y;
    return result;
}

double ae_c_abs(ae_complex z, ae_state *state)
{
    double w;
    double xabs;
    double yabs;
    double v;

    xabs = fabs(z.x);
    yabs = fabs(z.y);
    w = xabs>yabs ? xabs : yabs;
    v = xabs<yabs ? xabs : yabs;
    if( v==0 )
        return w;
    else
    {
        double t = v/w;
        return w*sqrt(1+t*t);
    }
}

ae_bool ae_c_eq(ae_complex lhs,   ae_complex rhs)
{
    volatile double x1 = lhs.x;
    volatile double x2 = rhs.x;
    volatile double y1 = lhs.y;
    volatile double y2 = rhs.y;
    return x1==x2 && y1==y2;
}

ae_bool ae_c_neq(ae_complex lhs,  ae_complex rhs)
{
    volatile double x1 = lhs.x;
    volatile double x2 = rhs.x;
    volatile double y1 = lhs.y;
    volatile double y2 = rhs.y;
    return x1!=x2 || y1!=y2;
}

ae_complex ae_c_add(ae_complex lhs,  ae_complex rhs)
{
    ae_complex result;
    result.x = lhs.x+rhs.x;
    result.y = lhs.y+rhs.y;
    return result;
}

ae_complex ae_c_mul(ae_complex lhs,  ae_complex rhs)
{
    ae_complex result;
    result.x = lhs.x*rhs.x-lhs.y*rhs.y;
    result.y = lhs.x*rhs.y+lhs.y*rhs.x;
    return result;
}

ae_complex ae_c_sub(ae_complex lhs,   ae_complex rhs)
{
    ae_complex result;
    result.x = lhs.x-rhs.x;
    result.y = lhs.y-rhs.y;
    return result;
}

ae_complex ae_c_div(ae_complex lhs,   ae_complex rhs)
{
    ae_complex result;
    double e;
    double f;
    if( fabs(rhs.y)<fabs(rhs.x) )
    {
        e = rhs.y/rhs.x;
        f = rhs.x+rhs.y*e;
        result.x = (lhs.x+lhs.y*e)/f;
        result.y = (lhs.y-lhs.x*e)/f;
    }
    else
    {
        e = rhs.x/rhs.y;
        f = rhs.y+rhs.x*e;
        result.x = (lhs.y+lhs.x*e)/f;
        result.y = (-lhs.x+lhs.y*e)/f;
    }
    return result;
}

ae_bool ae_c_eq_d(ae_complex lhs,  double rhs)
{
    volatile double x1 = lhs.x;
    volatile double x2 = rhs;
    volatile double y1 = lhs.y;
    volatile double y2 = 0;
    return x1==x2 && y1==y2;
}

ae_bool ae_c_neq_d(ae_complex lhs, double rhs)
{
    volatile double x1 = lhs.x;
    volatile double x2 = rhs;
    volatile double y1 = lhs.y;
    volatile double y2 = 0;
    return x1!=x2 || y1!=y2;
}

ae_complex ae_c_add_d(ae_complex lhs, double rhs)
{
    ae_complex result;
    result.x = lhs.x+rhs;
    result.y = lhs.y;
    return result;
}

ae_complex ae_c_mul_d(ae_complex lhs, double rhs)
{
    ae_complex result;
    result.x = lhs.x*rhs;
    result.y = lhs.y*rhs;
    return result;
}

ae_complex ae_c_sub_d(ae_complex lhs, double rhs)
{
    ae_complex result;
    result.x = lhs.x-rhs;
    result.y = lhs.y;
    return result;
}

ae_complex ae_c_d_sub(double lhs,     ae_complex rhs)
{
    ae_complex result;
    result.x = lhs-rhs.x;
    result.y = -rhs.y;
    return result;
}

ae_complex ae_c_div_d(ae_complex lhs, double rhs)
{
    ae_complex result;
    result.x = lhs.x/rhs;
    result.y = lhs.y/rhs;
    return result;
}

ae_complex ae_c_d_div(double lhs,   ae_complex rhs)
{
    ae_complex result;
    double e;
    double f;
    if( fabs(rhs.y)<fabs(rhs.x) )
    {
        e = rhs.y/rhs.x;
        f = rhs.x+rhs.y*e;
        result.x = lhs/f;
        result.y = -lhs*e/f;
    }
    else
    {
        e = rhs.x/rhs.y;
        f = rhs.y+rhs.x*e;
        result.x = lhs*e/f;
        result.y = -lhs/f;
    }
    return result;
}


/************************************************************************
Complex BLAS operations
************************************************************************/
ae_complex ae_v_cdotproduct(const ae_complex *v0, ae_int_t stride0, const char *conj0, const ae_complex *v1, ae_int_t stride1, const char *conj1, ae_int_t n)
{
    double rx = 0, ry = 0; 
    ae_int_t i;
    ae_bool bconj0 = !((conj0[0]=='N') || (conj0[0]=='n'));
    ae_bool bconj1 = !((conj1[0]=='N') || (conj1[0]=='n'));
    ae_complex result;
    if( bconj0 && bconj1 )
    {
        double v0x, v0y, v1x, v1y;
        for(i=0; i<n; i++, v0+=stride0, v1+=stride1)
        {
            v0x = v0->x;
            v0y = -v0->y;
            v1x = v1->x;
            v1y = -v1->y;
            rx += v0x*v1x-v0y*v1y;
            ry += v0x*v1y+v0y*v1x;
        }
    }
    if( !bconj0 && bconj1 )
    {
        double v0x, v0y, v1x, v1y;
        for(i=0; i<n; i++, v0+=stride0, v1+=stride1)
        {
            v0x = v0->x;
            v0y = v0->y;
            v1x = v1->x;
            v1y = -v1->y;
            rx += v0x*v1x-v0y*v1y;
            ry += v0x*v1y+v0y*v1x;
        }
    }
    if( bconj0 && !bconj1 )
    {
        double v0x, v0y, v1x, v1y;
        for(i=0; i<n; i++, v0+=stride0, v1+=stride1)
        {
            v0x = v0->x;
            v0y = -v0->y;
            v1x = v1->x;
            v1y = v1->y;
            rx += v0x*v1x-v0y*v1y;
            ry += v0x*v1y+v0y*v1x;
        }
    }
    if( !bconj0 && !bconj1 )
    {
        double v0x, v0y, v1x, v1y;
        for(i=0; i<n; i++, v0+=stride0, v1+=stride1)
        {
            v0x = v0->x;
            v0y = v0->y;
            v1x = v1->x;
            v1y = v1->y;
            rx += v0x*v1x-v0y*v1y;
            ry += v0x*v1y+v0y*v1x;
        }
    }
    result.x = rx;
    result.y = ry;
    return result;
}

void ae_v_cmove(ae_complex *vdst, ae_int_t stride_dst, const ae_complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n)
{
    ae_bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x =  vsrc->x;
                vdst->y = -vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
                *vdst = *vsrc;
        }
    }
    else
    {
        /*
         * optimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x =  vsrc->x;
                vdst->y = -vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
                *vdst = *vsrc;
        }
    }
}

void ae_v_cmoveneg(ae_complex *vdst, ae_int_t stride_dst, const ae_complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n)
{
    ae_bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x = -vsrc->x;
                vdst->y =  vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x = -vsrc->x;
                vdst->y = -vsrc->y;
            }
        }
    }
    else
    {
        /*
         * optimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x = -vsrc->x;
                vdst->y =  vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x = -vsrc->x;
                vdst->y = -vsrc->y;
            }
        }
    }
}

void ae_v_cmoved(ae_complex *vdst, ae_int_t stride_dst, const ae_complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha)
{
    ae_bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x =  alpha*vsrc->x;
                vdst->y = -alpha*vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x = alpha*vsrc->x;
                vdst->y = alpha*vsrc->y;
            }
        }
    }
    else
    {
        /*
         * optimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x =  alpha*vsrc->x;
                vdst->y = -alpha*vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x = alpha*vsrc->x;
                vdst->y = alpha*vsrc->y;
            }
        }
    }
}

void ae_v_cmovec(ae_complex *vdst, ae_int_t stride_dst, const ae_complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, ae_complex alpha)
{
    ae_bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        if( bconj )
        {
            double ax = alpha.x, ay = alpha.y;
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x =  ax*vsrc->x+ay*vsrc->y;
                vdst->y = -ax*vsrc->y+ay*vsrc->x;
            }
        }
        else
        {
            double ax = alpha.x, ay = alpha.y;
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x = ax*vsrc->x-ay*vsrc->y;
                vdst->y = ax*vsrc->y+ay*vsrc->x;
            }
        }
    }
    else
    {
        /*
         * highly optimized case
         */
        if( bconj )
        {
            double ax = alpha.x, ay = alpha.y;
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x =  ax*vsrc->x+ay*vsrc->y;
                vdst->y = -ax*vsrc->y+ay*vsrc->x;
            }
        }
        else
        {
            double ax = alpha.x, ay = alpha.y;
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x = ax*vsrc->x-ay*vsrc->y;
                vdst->y = ax*vsrc->y+ay*vsrc->x;
            }
        }
    }
}

void ae_v_cadd(ae_complex *vdst,     ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n)
{
    ae_bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += vsrc->x;
                vdst->y -= vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += vsrc->x;
                vdst->y += vsrc->y;
            }
        }
    }
    else
    {
        /*
         * optimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += vsrc->x;
                vdst->y -= vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += vsrc->x;
                vdst->y += vsrc->y;
            }
        }
    }
}

void ae_v_caddd(ae_complex *vdst,    ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha)
{
    ae_bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += alpha*vsrc->x;
                vdst->y -= alpha*vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += alpha*vsrc->x;
                vdst->y += alpha*vsrc->y;
            }
        }
    }
    else
    {
        /*
         * optimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += alpha*vsrc->x;
                vdst->y -= alpha*vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += alpha*vsrc->x;
                vdst->y += alpha*vsrc->y;
            }
        }
    }
}

void ae_v_caddc(ae_complex *vdst,    ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, ae_complex alpha)
{
    ae_bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        double ax = alpha.x, ay = alpha.y;
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += ax*vsrc->x+ay*vsrc->y;
                vdst->y -= ax*vsrc->y-ay*vsrc->x;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += ax*vsrc->x-ay*vsrc->y;
                vdst->y += ax*vsrc->y+ay*vsrc->x;
            }
        }
    }
    else
    {
        /*
         * highly optimized case
         */
        double ax = alpha.x, ay = alpha.y;
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += ax*vsrc->x+ay*vsrc->y;
                vdst->y -= ax*vsrc->y-ay*vsrc->x;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += ax*vsrc->x-ay*vsrc->y;
                vdst->y += ax*vsrc->y+ay*vsrc->x;
            }
        }
    }
}

void ae_v_csub(ae_complex *vdst,     ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n)
{
    ae_bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x -= vsrc->x;
                vdst->y += vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x -= vsrc->x;
                vdst->y -= vsrc->y;
            }
        }
    }
    else
    {
        /*
         * highly optimized case
         */
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x -= vsrc->x;
                vdst->y += vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x -= vsrc->x;
                vdst->y -= vsrc->y;
            }
        }
    }
}

void ae_v_csubd(ae_complex *vdst, ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha)
{
    ae_v_caddd(vdst, stride_dst, vsrc, stride_src, conj_src, n, -alpha);
}

void ae_v_csubc(ae_complex *vdst, ae_int_t stride_dst, const ae_complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, ae_complex alpha)
{
    alpha.x = -alpha.x;
    alpha.y = -alpha.y;
    ae_v_caddc(vdst, stride_dst, vsrc, stride_src, conj_src, n, alpha);
}

void ae_v_cmuld(ae_complex *vdst, ae_int_t stride_dst, ae_int_t n, double alpha)
{
    ae_int_t i;
    if( stride_dst!=1 )
    {
        /*
         * general unoptimized case
         */
        for(i=0; i<n; i++, vdst+=stride_dst)
        {
            vdst->x *= alpha;
            vdst->y *= alpha;
        }
    }
    else
    {
        /*
         * optimized case
         */
        for(i=0; i<n; i++, vdst++)
        {
            vdst->x *= alpha;
            vdst->y *= alpha;
        }
    }
}

void ae_v_cmulc(ae_complex *vdst, ae_int_t stride_dst, ae_int_t n, ae_complex alpha)
{
    ae_int_t i;
    if( stride_dst!=1 )
    {
        /*
         * general unoptimized case
         */
        double ax = alpha.x, ay = alpha.y;
        for(i=0; i<n; i++, vdst+=stride_dst)
        {
            double  dstx = vdst->x, dsty = vdst->y;
            vdst->x = ax*dstx-ay*dsty;
            vdst->y = ax*dsty+ay*dstx;
        }
    }
    else
    {
        /*
         * highly optimized case
         */
        double ax = alpha.x, ay = alpha.y;
        for(i=0; i<n; i++, vdst++)
        {
            double  dstx = vdst->x, dsty = vdst->y;
            vdst->x = ax*dstx-ay*dsty;
            vdst->y = ax*dsty+ay*dstx;
        }
    }
}

/************************************************************************
Real BLAS operations
************************************************************************/
double ae_v_dotproduct(const double *v0, ae_int_t stride0, const double *v1, ae_int_t stride1, ae_int_t n)
{
    double result = 0;
    ae_int_t i;
    if( stride0!=1 || stride1!=1 )
    {
        /*
         * slow general code
         */
        for(i=0; i<n; i++, v0+=stride0, v1+=stride1)
            result += (*v0)*(*v1);
    }
    else
    {
        /*
         * optimized code for stride=1
         */
        ae_int_t n4 = n/4;
        ae_int_t nleft = n%4;
        for(i=0; i<n4; i++, v0+=4, v1+=4)
            result += v0[0]*v1[0]+v0[1]*v1[1]+v0[2]*v1[2]+v0[3]*v1[3];
        for(i=0; i<nleft; i++, v0++, v1++)
            result += v0[0]*v1[0];
    }
    return result;
}

void ae_v_move(double *vdst,  ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst = *vsrc;
    }
    else
    {
        /*
         * optimized case
         */
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] = vsrc[0];
            vdst[1] = vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] = vsrc[0];
    }
}

void ae_v_moveneg(double *vdst,  ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst = -*vsrc;
    }
    else
    {
        /*
         * optimized case
         */
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] = -vsrc[0];
            vdst[1] = -vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] = -vsrc[0];
    }
}

void ae_v_moved(double *vdst,  ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n, double alpha)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst = alpha*(*vsrc);
    }
    else
    {
        /*
         * optimized case
         */
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] = alpha*vsrc[0];
            vdst[1] = alpha*vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] = alpha*vsrc[0];
    }
}

void ae_v_add(double *vdst,     ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst += *vsrc;
    }
    else
    {
        /*
         * optimized case
         */
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] += vsrc[0];
            vdst[1] += vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] += vsrc[0];
    }
}

void ae_v_addd(double *vdst,    ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n, double alpha)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst += alpha*(*vsrc);
    }
    else
    {
        /*
         * optimized case
         */
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] += alpha*vsrc[0];
            vdst[1] += alpha*vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] += alpha*vsrc[0];
    }
}

void ae_v_sub(double *vdst,     ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        /*
         * general unoptimized case
         */
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst -= *vsrc;
    }
    else
    {
        /*
         * highly optimized case
         */
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] -= vsrc[0];
            vdst[1] -= vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] -= vsrc[0];
    }
}

void ae_v_subd(double *vdst,  ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n, double alpha)
{
    ae_v_addd(vdst, stride_dst, vsrc, stride_src, n, -alpha);
}

void ae_v_muld(double *vdst,  ae_int_t stride_dst, ae_int_t n, double alpha)
{
    ae_int_t i;
    if( stride_dst!=1 )
    {
        /*
         * general unoptimized case
         */
        for(i=0; i<n; i++, vdst+=stride_dst)
            *vdst *= alpha;
    }
    else
    {
        /*
         * highly optimized case
         */
        for(i=0; i<n; i++, vdst++)
            *vdst *= alpha;
    }
}

/************************************************************************
Other functions
************************************************************************/
ae_int_t ae_v_len(ae_int_t a, ae_int_t b)
{
    return b-a+1;
}

/************************************************************************
RComm functions
************************************************************************/
ae_bool _rcommstate_init(rcommstate* p, ae_state *_state, ae_bool make_automatic)
{
    if( !ae_vector_init(&p->ba, 0, DT_BOOL,    _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->ia, 0, DT_INT,     _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->ra, 0, DT_REAL,    _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init(&p->ca, 0, DT_COMPLEX, _state, make_automatic) )
        return ae_false;
    return ae_true;
}

ae_bool _rcommstate_init_copy(rcommstate* dst, rcommstate* src, ae_state *_state, ae_bool make_automatic)
{
    if( !ae_vector_init_copy(&dst->ba, &src->ba, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->ia, &src->ia, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->ra, &src->ra, _state, make_automatic) )
        return ae_false;
    if( !ae_vector_init_copy(&dst->ca, &src->ca, _state, make_automatic) )
        return ae_false;
    dst->stage = src->stage;
    return ae_true;
}

void _rcommstate_clear(rcommstate* p)
{
    ae_vector_clear(&p->ba);
    ae_vector_clear(&p->ia);
    ae_vector_clear(&p->ra);
    ae_vector_clear(&p->ca);
}

}

/////////////////////////////////////////////////////////////////////////
//
// THIS SECTION CONTAINS C++ RELATED FUNCTIONALITY
//
/////////////////////////////////////////////////////////////////////////
/********************************************************************
Internal forwards
********************************************************************/
namespace alglib
{
    double get_aenv_nan();
    double get_aenv_posinf();
    double get_aenv_neginf();
    ae_int_t my_stricmp(const char *s1, const char *s2);
    char* filter_spaces(const char *s);
    void str_vector_create(const char *src, bool match_head_only, std::vector<const char*> *p_vec);
    void str_matrix_create(const char *src, std::vector< std::vector<const char*> > *p_mat);
    
    ae_bool parse_bool_delim(const char *s, const char *delim);
    ae_int_t parse_int_delim(const char *s, const char *delim);
    bool _parse_real_delim(const char *s, const char *delim, double *result, const char **new_s);
    double parse_real_delim(const char *s, const char *delim);
    alglib::complex parse_complex_delim(const char *s, const char *delim);

    std::string arraytostring(const bool *ptr, ae_int_t n);
    std::string arraytostring(const ae_int_t *ptr, ae_int_t n);
    std::string arraytostring(const double *ptr, ae_int_t n, int dps);
    std::string arraytostring(const alglib::complex *ptr, ae_int_t n, int dps);
}

/********************************************************************
Global and local constants
********************************************************************/
const double alglib::machineepsilon = 5E-16;
const double alglib::maxrealnumber  = 1E300;
const double alglib::minrealnumber  = 1E-300;
const alglib::ae_int_t alglib::endianness =  alglib_impl::ae_get_endianness();
const double alglib::fp_nan         =  alglib::get_aenv_nan();
const double alglib::fp_posinf      =  alglib::get_aenv_posinf();
const double alglib::fp_neginf      =  alglib::get_aenv_neginf();


/********************************************************************
ap_error
********************************************************************/
alglib::ap_error::ap_error()
{
}

alglib::ap_error::ap_error(const char *s)
{
    msg = s; 
}

void alglib::ap_error::make_assertion(bool bClause)
{
    if(!bClause) 
        throw ap_error(); 
}

void alglib::ap_error::make_assertion(bool bClause, const char *msg)
{ 
    if(!bClause) 
        throw ap_error(msg); 
}


/********************************************************************
Complex number with double precision.
********************************************************************/
alglib::complex::complex():x(0.0),y(0.0)
{
}

alglib::complex::complex(const double &_x):x(_x),y(0.0)
{
}

alglib::complex::complex(const double &_x, const double &_y):x(_x),y(_y)
{
}

alglib::complex::complex(const alglib::complex &z):x(z.x),y(z.y)
{
}

alglib::complex& alglib::complex::operator= (const double& v)
{
    x = v; 
    y = 0.0; 
    return *this; 
}

alglib::complex& alglib::complex::operator+=(const double& v)
{
    x += v;
    return *this; 
}

alglib::complex& alglib::complex::operator-=(const double& v)
{
    x -= v;
    return *this;
}

alglib::complex& alglib::complex::operator*=(const double& v)
{
    x *= v;
    y *= v;
    return *this; 
}

alglib::complex& alglib::complex::operator/=(const double& v)
{
    x /= v;
    y /= v;
    return *this;
}

alglib::complex& alglib::complex::operator= (const alglib::complex& z)
{
    x = z.x;
    y = z.y;
    return *this;
}

alglib::complex& alglib::complex::operator+=(const alglib::complex& z)
{
    x += z.x;
    y += z.y;
    return *this;
}

alglib::complex& alglib::complex::operator-=(const alglib::complex& z)
{
    x -= z.x;
    y -= z.y;
    return *this;
}

alglib::complex& alglib::complex::operator*=(const alglib::complex& z)
{
    double t = x*z.x-y*z.y;
    y = x*z.y+y*z.x;
    x = t; 
    return *this;
}

alglib::complex& alglib::complex::operator/=(const alglib::complex& z)
{
    alglib::complex result;
    double e;
    double f;
    if( fabs(z.y)<fabs(z.x) )
    {
        e = z.y/z.x;
        f = z.x+z.y*e;
        result.x = (x+y*e)/f;
        result.y = (y-x*e)/f;
    }
    else
    {
        e = z.x/z.y;
        f = z.y+z.x*e;
        result.x = (y+x*e)/f;
        result.y = (-x+y*e)/f;
    }
    *this = result;
    return *this;
}

alglib_impl::ae_complex* alglib::complex::c_ptr()
{
    return (alglib_impl::ae_complex*)this;
}

const alglib_impl::ae_complex* alglib::complex::c_ptr() const
{
    return (const alglib_impl::ae_complex*)this;
}
    
std::string alglib::complex::tostring(int dps) const
{
    char mask[32];
    char buf_x[32];
    char buf_y[32];
    char buf_zero[32];
    if( dps<=0 || dps>=20 )
        throw ap_error("complex::tostring(): incorrect dps");

    // handle IEEE special quantities
    if( fp_isnan(x) || fp_isnan(y) )
        return "NAN";
    if( fp_isinf(x) || fp_isinf(y) )
        return "INF";

    // generate mask
    if( sprintf(mask, "%%.%df", dps)>=(int)sizeof(mask) )
        throw ap_error("complex::tostring(): buffer overflow");

    // print |x|, |y| and zero with same mask and compare
    if( sprintf(buf_x, mask, (double)(fabs(x)))>=(int)sizeof(buf_x) )
        throw ap_error("complex::tostring(): buffer overflow");
    if( sprintf(buf_y, mask, (double)(fabs(y)))>=(int)sizeof(buf_y) )
        throw ap_error("complex::tostring(): buffer overflow");
    if( sprintf(buf_zero, mask, (double)0)>=(int)sizeof(buf_zero) )
        throw ap_error("complex::tostring(): buffer overflow");

    // different zero/nonzero patterns
    if( strcmp(buf_x,buf_zero)!=0 && strcmp(buf_y,buf_zero)!=0 )
        return std::string(x>0 ? "" : "-")+buf_x+(y>0 ? "+" : "-")+buf_y+"i";
    if( strcmp(buf_x,buf_zero)!=0 && strcmp(buf_y,buf_zero)==0 )
        return std::string(x>0 ? "" : "-")+buf_x;
    if( strcmp(buf_x,buf_zero)==0 && strcmp(buf_y,buf_zero)!=0 )
        return std::string(y>0 ? "" : "-")+buf_y+"i";
    return std::string("0");
}

const bool alglib::operator==(const alglib::complex& lhs, const alglib::complex& rhs)
{
    volatile double x1 = lhs.x;
    volatile double x2 = rhs.x;
    volatile double y1 = lhs.y;
    volatile double y2 = rhs.y;
    return x1==x2 && y1==y2;
}

const bool alglib::operator!=(const alglib::complex& lhs, const alglib::complex& rhs)
{ return !(lhs==rhs); }

const alglib::complex alglib::operator+(const alglib::complex& lhs)
{ return lhs; }

const alglib::complex alglib::operator-(const alglib::complex& lhs)
{ return alglib::complex(-lhs.x, -lhs.y); }

const alglib::complex alglib::operator+(const alglib::complex& lhs, const alglib::complex& rhs)
{ alglib::complex r = lhs; r += rhs; return r; }

const alglib::complex alglib::operator+(const alglib::complex& lhs, const double& rhs)
{ alglib::complex r = lhs; r += rhs; return r; }

const alglib::complex alglib::operator+(const double& lhs, const alglib::complex& rhs)
{ alglib::complex r = rhs; r += lhs; return r; }

const alglib::complex alglib::operator-(const alglib::complex& lhs, const alglib::complex& rhs)
{ alglib::complex r = lhs; r -= rhs; return r; }

const alglib::complex alglib::operator-(const alglib::complex& lhs, const double& rhs)
{ alglib::complex r = lhs; r -= rhs; return r; }

const alglib::complex alglib::operator-(const double& lhs, const alglib::complex& rhs)
{ alglib::complex r = lhs; r -= rhs; return r; }

const alglib::complex alglib::operator*(const alglib::complex& lhs, const alglib::complex& rhs)
{ return alglib::complex(lhs.x*rhs.x - lhs.y*rhs.y,  lhs.x*rhs.y + lhs.y*rhs.x); }

const alglib::complex alglib::operator*(const alglib::complex& lhs, const double& rhs)
{ return alglib::complex(lhs.x*rhs,  lhs.y*rhs); }

const alglib::complex alglib::operator*(const double& lhs, const alglib::complex& rhs)
{ return alglib::complex(lhs*rhs.x,  lhs*rhs.y); }

const alglib::complex alglib::operator/(const alglib::complex& lhs, const alglib::complex& rhs)
{
    alglib::complex result;
    double e;
    double f;
    if( fabs(rhs.y)<fabs(rhs.x) )
    {
        e = rhs.y/rhs.x;
        f = rhs.x+rhs.y*e;
        result.x = (lhs.x+lhs.y*e)/f;
        result.y = (lhs.y-lhs.x*e)/f;
    }
    else
    {
        e = rhs.x/rhs.y;
        f = rhs.y+rhs.x*e;
        result.x = (lhs.y+lhs.x*e)/f;
        result.y = (-lhs.x+lhs.y*e)/f;
    }
    return result;
}

const alglib::complex alglib::operator/(const double& lhs, const alglib::complex& rhs)
{
    alglib::complex result;
    double e;
    double f;
    if( fabs(rhs.y)<fabs(rhs.x) )
    {
        e = rhs.y/rhs.x;
        f = rhs.x+rhs.y*e;
        result.x = lhs/f;
        result.y = -lhs*e/f;
    }
    else
    {
        e = rhs.x/rhs.y;
        f = rhs.y+rhs.x*e;
        result.x = lhs*e/f;
        result.y = -lhs/f;
    }
    return result;
}

const alglib::complex alglib::operator/(const alglib::complex& lhs, const double& rhs)
{ return alglib::complex(lhs.x/rhs, lhs.y/rhs); }

double alglib::abscomplex(const alglib::complex &z)
{
    double w;
    double xabs;
    double yabs;
    double v;

    xabs = fabs(z.x);
    yabs = fabs(z.y);
    w = xabs>yabs ? xabs : yabs;
    v = xabs<yabs ? xabs : yabs; 
    if( v==0 )
        return w;
    else
    {
        double t = v/w;
        return w*sqrt(1+t*t);
    }
}

alglib::complex alglib::conj(const alglib::complex &z)
{ return alglib::complex(z.x, -z.y); }

alglib::complex alglib::csqr(const alglib::complex &z)
{ return alglib::complex(z.x*z.x-z.y*z.y, 2*z.x*z.y); }


/********************************************************************
Level 1 BLAS functions
********************************************************************/
double alglib::vdotproduct(const double *v0, ae_int_t stride0, const double *v1, ae_int_t stride1, ae_int_t n)
{
    double result = 0;
    ae_int_t i;
    if( stride0!=1 || stride1!=1 )
    {
        //
        // slow general code
        //
        for(i=0; i<n; i++, v0+=stride0, v1+=stride1)
            result += (*v0)*(*v1);
    }
    else
    {
        //
        // optimized code for stride=1
        //
        ae_int_t n4 = n/4;
        ae_int_t nleft = n%4;
        for(i=0; i<n4; i++, v0+=4, v1+=4)
            result += v0[0]*v1[0]+v0[1]*v1[1]+v0[2]*v1[2]+v0[3]*v1[3];
        for(i=0; i<nleft; i++, v0++, v1++)
            result += v0[0]*v1[0];
    }
    return result;
}

double alglib::vdotproduct(const double *v1, const double *v2, ae_int_t N)
{
    return vdotproduct(v1, 1, v2, 1, N);
}

alglib::complex alglib::vdotproduct(const alglib::complex *v0, ae_int_t stride0, const char *conj0, const alglib::complex *v1, ae_int_t stride1, const char *conj1, ae_int_t n)
{
    double rx = 0, ry = 0;
    ae_int_t i;
    bool bconj0 = !((conj0[0]=='N') || (conj0[0]=='n'));
    bool bconj1 = !((conj1[0]=='N') || (conj1[0]=='n'));
    if( bconj0 && bconj1 )
    {
        double v0x, v0y, v1x, v1y;
        for(i=0; i<n; i++, v0+=stride0, v1+=stride1)
        {
            v0x = v0->x;
            v0y = -v0->y;
            v1x = v1->x;
            v1y = -v1->y;
            rx += v0x*v1x-v0y*v1y;
            ry += v0x*v1y+v0y*v1x;
        }
    }
    if( !bconj0 && bconj1 )
    {
        double v0x, v0y, v1x, v1y;
        for(i=0; i<n; i++, v0+=stride0, v1+=stride1)
        {
            v0x = v0->x;
            v0y = v0->y;
            v1x = v1->x;
            v1y = -v1->y;
            rx += v0x*v1x-v0y*v1y;
            ry += v0x*v1y+v0y*v1x;
        }
    }
    if( bconj0 && !bconj1 )
    {
        double v0x, v0y, v1x, v1y;
        for(i=0; i<n; i++, v0+=stride0, v1+=stride1)
        {
            v0x = v0->x;
            v0y = -v0->y;
            v1x = v1->x;
            v1y = v1->y;
            rx += v0x*v1x-v0y*v1y;
            ry += v0x*v1y+v0y*v1x;
        }
    }
    if( !bconj0 && !bconj1 )
    {
        double v0x, v0y, v1x, v1y;
        for(i=0; i<n; i++, v0+=stride0, v1+=stride1)
        {
            v0x = v0->x;
            v0y = v0->y;
            v1x = v1->x;
            v1y = v1->y;
            rx += v0x*v1x-v0y*v1y;
            ry += v0x*v1y+v0y*v1x;
        }
    }
    return alglib::complex(rx,ry);
}

alglib::complex alglib::vdotproduct(const alglib::complex *v1, const alglib::complex *v2, ae_int_t N)
{
    return vdotproduct(v1, 1, "N", v2, 1, "N", N);
}

void alglib::vmove(double *vdst, ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst = *vsrc;
    }
    else
    {
        //
        // optimized case
        //
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] = vsrc[0];
            vdst[1] = vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] = vsrc[0];
    }
}

void alglib::vmove(double *vdst, const double* vsrc, ae_int_t N)
{
    vmove(vdst, 1, vsrc, 1, N);
}

void alglib::vmove(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n)
{
    bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x =  vsrc->x;
                vdst->y = -vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
                *vdst = *vsrc;
        }
    }
    else
    {
        //
        // optimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x =  vsrc->x;
                vdst->y = -vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
                *vdst = *vsrc;
        }
    }
}

void alglib::vmove(alglib::complex *vdst, const alglib::complex* vsrc, ae_int_t N)
{
    vmove(vdst, 1, vsrc, 1, "N", N);
}

void alglib::vmoveneg(double *vdst,  ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst = -*vsrc;
    }
    else
    {
        //
        // optimized case
        //
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] = -vsrc[0];
            vdst[1] = -vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] = -vsrc[0];
    }
}

void alglib::vmoveneg(double *vdst, const double *vsrc, ae_int_t N)
{
    vmoveneg(vdst, 1, vsrc, 1, N);
}

void alglib::vmoveneg(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n)
{
    bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x = -vsrc->x;
                vdst->y =  vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x = -vsrc->x;
                vdst->y = -vsrc->y;
            }
        }
    }
    else
    {
        //
        // optimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x = -vsrc->x;
                vdst->y =  vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x = -vsrc->x;
                vdst->y = -vsrc->y;
            }
        }
    }
}

void alglib::vmoveneg(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N)
{
    vmoveneg(vdst, 1, vsrc, 1, "N", N);
}

void alglib::vmove(double *vdst,  ae_int_t stride_dst, const double* vsrc,  ae_int_t stride_src, ae_int_t n, double alpha)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst = alpha*(*vsrc);
    }
    else
    {
        //
        // optimized case
        //
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] = alpha*vsrc[0];
            vdst[1] = alpha*vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] = alpha*vsrc[0];
    }
}

void alglib::vmove(double *vdst, const double *vsrc, ae_int_t N, double alpha)
{
    vmove(vdst, 1, vsrc, 1, N, alpha);
}

void alglib::vmove(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha)
{
    bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x =  alpha*vsrc->x;
                vdst->y = -alpha*vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x = alpha*vsrc->x;
                vdst->y = alpha*vsrc->y;
            }
        }
    }
    else
    {
        //
        // optimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x =  alpha*vsrc->x;
                vdst->y = -alpha*vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x = alpha*vsrc->x;
                vdst->y = alpha*vsrc->y;
            }
        }
    }
}

void alglib::vmove(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N, double alpha)
{
    vmove(vdst, 1, vsrc, 1, "N", N, alpha);
}

void alglib::vmove(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex* vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, alglib::complex alpha)
{
    bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        if( bconj )
        {
            double ax = alpha.x, ay = alpha.y;
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x =  ax*vsrc->x+ay*vsrc->y;
                vdst->y = -ax*vsrc->y+ay*vsrc->x;
            }
        }
        else
        {
            double ax = alpha.x, ay = alpha.y;
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x = ax*vsrc->x-ay*vsrc->y;
                vdst->y = ax*vsrc->y+ay*vsrc->x;
            }
        }
    }
    else
    {
        //
        // optimized case
        //
        if( bconj )
        {
            double ax = alpha.x, ay = alpha.y;
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x =  ax*vsrc->x+ay*vsrc->y;
                vdst->y = -ax*vsrc->y+ay*vsrc->x;
            }
        }
        else
        {
            double ax = alpha.x, ay = alpha.y;
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x = ax*vsrc->x-ay*vsrc->y;
                vdst->y = ax*vsrc->y+ay*vsrc->x;
            }
        }
    }
}

void alglib::vmove(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N, alglib::complex alpha)
{
    vmove(vdst, 1, vsrc, 1, "N", N, alpha);
}

void alglib::vadd(double *vdst,  ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst += *vsrc;
    }
    else
    {
        //
        // optimized case
        //
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] += vsrc[0];
            vdst[1] += vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] += vsrc[0];
    }
}

void alglib::vadd(double *vdst, const double *vsrc, ae_int_t N)
{
    vadd(vdst, 1, vsrc, 1, N);
}

void alglib::vadd(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n)
{
    bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += vsrc->x;
                vdst->y -= vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += vsrc->x;
                vdst->y += vsrc->y;
            }
        }
    }
    else
    {
        //
        // optimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += vsrc->x;
                vdst->y -= vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += vsrc->x;
                vdst->y += vsrc->y;
            }
        }
    }
}

void alglib::vadd(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N)
{
    vadd(vdst, 1, vsrc, 1, "N", N);
}

void alglib::vadd(double *vdst,  ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n, double alpha)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst += alpha*(*vsrc);
    }
    else
    {
        //
        // optimized case
        //
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] += alpha*vsrc[0];
            vdst[1] += alpha*vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] += alpha*vsrc[0];
    }
}

void alglib::vadd(double *vdst, const double *vsrc, ae_int_t N, double alpha)
{
    vadd(vdst, 1, vsrc, 1, N, alpha);
}

void alglib::vadd(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha)
{
    bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += alpha*vsrc->x;
                vdst->y -= alpha*vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += alpha*vsrc->x;
                vdst->y += alpha*vsrc->y;
            }
        }
    }
    else
    {
        //
        // optimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += alpha*vsrc->x;
                vdst->y -= alpha*vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += alpha*vsrc->x;
                vdst->y += alpha*vsrc->y;
            }
        }
    }
}

void alglib::vadd(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N, double alpha)
{
    vadd(vdst, 1, vsrc, 1, "N", N, alpha);
}

void alglib::vadd(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, alglib::complex alpha)
{
    bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        double ax = alpha.x, ay = alpha.y;
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += ax*vsrc->x+ay*vsrc->y;
                vdst->y -= ax*vsrc->y-ay*vsrc->x;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x += ax*vsrc->x-ay*vsrc->y;
                vdst->y += ax*vsrc->y+ay*vsrc->x;
            }
        }
    }
    else
    {
        //
        // optimized case
        //
        double ax = alpha.x, ay = alpha.y;
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += ax*vsrc->x+ay*vsrc->y;
                vdst->y -= ax*vsrc->y-ay*vsrc->x;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x += ax*vsrc->x-ay*vsrc->y;
                vdst->y += ax*vsrc->y+ay*vsrc->x;
            }
        }
    }
}

void alglib::vadd(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N, alglib::complex alpha)
{
    vadd(vdst, 1, vsrc, 1, "N", N, alpha);
}

void alglib::vsub(double *vdst,  ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n)
{
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            *vdst -= *vsrc;
    }
    else
    {
        //
        // optimized case
        //
        ae_int_t n2 = n/2;
        for(i=0; i<n2; i++, vdst+=2, vsrc+=2)
        {
            vdst[0] -= vsrc[0];
            vdst[1] -= vsrc[1];
        }
        if( n%2!=0 )
            vdst[0] -= vsrc[0];
    }
}

void alglib::vsub(double *vdst, const double *vsrc, ae_int_t N)
{
    vsub(vdst, 1, vsrc, 1, N);
}

void alglib::vsub(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n)
{
    bool bconj = !((conj_src[0]=='N') || (conj_src[0]=='n'));
    ae_int_t i;
    if( stride_dst!=1 || stride_src!=1 )
    {
        //
        // general unoptimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x -= vsrc->x;
                vdst->y += vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst+=stride_dst, vsrc+=stride_src)
            {
                vdst->x -= vsrc->x;
                vdst->y -= vsrc->y;
            }
        }
    }
    else
    {
        //
        // optimized case
        //
        if( bconj )
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x -= vsrc->x;
                vdst->y += vsrc->y;
            }
        }
        else
        {
            for(i=0; i<n; i++, vdst++, vsrc++)
            {
                vdst->x -= vsrc->x;
                vdst->y -= vsrc->y;
            }
        }
    }
}

void alglib::vsub(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t N)
{
    vsub(vdst, 1, vsrc, 1, "N", N);
}

void alglib::vsub(double *vdst,  ae_int_t stride_dst, const double *vsrc,  ae_int_t stride_src, ae_int_t n, double alpha)
{
    vadd(vdst, stride_dst, vsrc, stride_src, n, -alpha);
}

void alglib::vsub(double *vdst, const double *vsrc, ae_int_t N, double alpha)
{
    vadd(vdst, 1, vsrc, 1, N, -alpha);
}

void alglib::vsub(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, double alpha)
{
    vadd(vdst, stride_dst, vsrc, stride_src, conj_src, n, -alpha);
}

void alglib::vsub(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t n, double alpha)
{
    vadd(vdst, 1, vsrc, 1, "N", n, -alpha);
}

void alglib::vsub(alglib::complex *vdst, ae_int_t stride_dst, const alglib::complex *vsrc, ae_int_t stride_src, const char *conj_src, ae_int_t n, alglib::complex alpha)
{
    vadd(vdst, stride_dst, vsrc, stride_src, conj_src, n, -alpha);
}

void alglib::vsub(alglib::complex *vdst, const alglib::complex *vsrc, ae_int_t n, alglib::complex alpha)
{
    vadd(vdst, 1, vsrc, 1, "N", n, -alpha);
}
void alglib::vmul(double *vdst,  ae_int_t stride_dst, ae_int_t n, double alpha)
{
    ae_int_t i;
    if( stride_dst!=1 )
    {
        //
        // general unoptimized case
        //
        for(i=0; i<n; i++, vdst+=stride_dst)
            *vdst *= alpha;
    }
    else
    {
        //
        // optimized case
        //
        for(i=0; i<n; i++, vdst++)
            *vdst *= alpha;
    }
}

void alglib::vmul(double *vdst, ae_int_t N, double alpha)
{
    vmul(vdst, 1, N, alpha);
}

void alglib::vmul(alglib::complex *vdst, ae_int_t stride_dst, ae_int_t n, double alpha)
{
    ae_int_t i;
    if( stride_dst!=1 )
    {
        //
        // general unoptimized case
        //
        for(i=0; i<n; i++, vdst+=stride_dst)
        {
            vdst->x *= alpha;
            vdst->y *= alpha;
        }
    }
    else
    {
        //
        // optimized case
        //
        for(i=0; i<n; i++, vdst++)
        {
            vdst->x *= alpha;
            vdst->y *= alpha;
        }
    }
}

void alglib::vmul(alglib::complex *vdst, ae_int_t N, double alpha)
{
    vmul(vdst, 1, N, alpha);
}

void alglib::vmul(alglib::complex *vdst, ae_int_t stride_dst, ae_int_t n, alglib::complex alpha)
{
    ae_int_t i;
    if( stride_dst!=1 )
    {
        //
        // general unoptimized case
        //
        double ax = alpha.x, ay = alpha.y;
        for(i=0; i<n; i++, vdst+=stride_dst)
        {
            double  dstx = vdst->x, dsty = vdst->y;
            vdst->x = ax*dstx-ay*dsty;
            vdst->y = ax*dsty+ay*dstx;
        }
    }
    else
    {
        //
        // optimized case
        //
        double ax = alpha.x, ay = alpha.y;
        for(i=0; i<n; i++, vdst++)
        {
            double  dstx = vdst->x, dsty = vdst->y;
            vdst->x = ax*dstx-ay*dsty;
            vdst->y = ax*dsty+ay*dstx;
        }
    }
}

void alglib::vmul(alglib::complex *vdst, ae_int_t N, alglib::complex alpha)
{
    vmul(vdst, 1, N, alpha);
}


/********************************************************************
Matrices and vectors
********************************************************************/
alglib::ae_vector_wrapper::ae_vector_wrapper()
{
    p_vec = NULL;
}

alglib::ae_vector_wrapper::~ae_vector_wrapper()
{
    if( p_vec==&vec )
        ae_vector_clear(p_vec);
}

alglib::ae_vector_wrapper::ae_vector_wrapper(const alglib::ae_vector_wrapper &rhs)
{
    if( rhs.p_vec!=NULL )
    {
        p_vec = &vec;
        if( !ae_vector_init_copy(p_vec, rhs.p_vec, NULL, ae_false) )
            throw alglib::ap_error("ALGLIB: malloc error!");
    }
    else
        p_vec = NULL;
}

const alglib::ae_vector_wrapper& alglib::ae_vector_wrapper::operator=(const alglib::ae_vector_wrapper &rhs)
{
    if( this==&rhs )
        return *this;
    if( p_vec==&vec )
        ae_vector_clear(p_vec);
    if( rhs.p_vec!=NULL )
    {
        p_vec = &vec;
        if( !ae_vector_init_copy(p_vec, rhs.p_vec, NULL, ae_false) )
            throw alglib::ap_error("ALGLIB: malloc error!");
    }
    else
        p_vec = NULL;
    return *this;
}

void alglib::ae_vector_wrapper::setlength(ae_int_t iLen)
{
    if( p_vec==NULL )
        throw alglib::ap_error("ALGLIB: setlength() error, p_vec==NULL (array was not correctly initialized)");
    if( p_vec!=&vec )
        throw alglib::ap_error("ALGLIB: setlength() error, p_vec!=&vec (attempt to resize frozen array)");
    if( !ae_vector_set_length(p_vec, iLen, NULL) )
        throw alglib::ap_error("ALGLIB: malloc error");
}

alglib::ae_int_t alglib::ae_vector_wrapper::length() const
{
    if( p_vec==NULL )
        return 0;
    return p_vec->cnt;
}

void alglib::ae_vector_wrapper::attach_to(alglib_impl::ae_vector *ptr)
{
    if( ptr==&vec )
        throw alglib::ap_error("ALGLIB: attempt to attach vector to itself");
    if( p_vec==&vec )
        ae_vector_clear(p_vec);
    p_vec = ptr;
}

void alglib::ae_vector_wrapper::allocate_own(ae_int_t size, alglib_impl::ae_datatype datatype)
{
    if( p_vec==&vec )
        ae_vector_clear(p_vec);
    p_vec = &vec;
    if( !ae_vector_init(p_vec, size, datatype, NULL, false) )
        throw alglib::ap_error("ALGLIB: malloc error");
}

const alglib_impl::ae_vector* alglib::ae_vector_wrapper::c_ptr() const
{
    return p_vec;
}

alglib_impl::ae_vector* alglib::ae_vector_wrapper::c_ptr()
{
    return p_vec;
}

alglib::boolean_1d_array::boolean_1d_array()  
{
    allocate_own(0, alglib_impl::DT_BOOL);
}

alglib::boolean_1d_array::boolean_1d_array(const char *s)  
{
    std::vector<const char*> svec;
    size_t i;
    char *p = filter_spaces(s);
    try
    {
        str_vector_create(p, true, &svec);
        allocate_own((ae_int_t)(svec.size()), alglib_impl::DT_BOOL);
        for(i=0; i<svec.size(); i++)
            operator()((ae_int_t)i) = parse_bool_delim(svec[i],",]");
        alglib_impl::ae_free(p);
    }
    catch(...)
    {
        alglib_impl::ae_free(p);
        throw;
    }
}

alglib::boolean_1d_array::boolean_1d_array(alglib_impl::ae_vector *p)  
{
    p_vec = NULL;
    attach_to(p);
}

alglib::boolean_1d_array::~boolean_1d_array() 
{
}

const ae_bool& alglib::boolean_1d_array::operator()(ae_int_t i) const
{
    return p_vec->ptr.p_bool[i];
}

ae_bool& alglib::boolean_1d_array::operator()(ae_int_t i)
{
    return p_vec->ptr.p_bool[i];
}

const ae_bool& alglib::boolean_1d_array::operator[](ae_int_t i) const
{
    return p_vec->ptr.p_bool[i];
}

ae_bool& alglib::boolean_1d_array::operator[](ae_int_t i)
{
    return p_vec->ptr.p_bool[i];
}

void alglib::boolean_1d_array::setcontent(ae_int_t iLen, const bool *pContent )
{
    ae_int_t i;
    setlength(iLen);
    for(i=0; i<iLen; i++)
        p_vec->ptr.p_bool[i] = pContent[i];
}

ae_bool* alglib::boolean_1d_array::getcontent()
{
    return p_vec->ptr.p_bool;
}

const ae_bool* alglib::boolean_1d_array::getcontent() const
{
    return p_vec->ptr.p_bool;
}

std::string alglib::boolean_1d_array::tostring() const 
{
    if( length()==0 )
        return "[]";
    return arraytostring(&(operator()(0)), length());
}

alglib::integer_1d_array::integer_1d_array()  
{
    allocate_own(0, alglib_impl::DT_INT);
}

alglib::integer_1d_array::integer_1d_array(alglib_impl::ae_vector *p)  
{
    p_vec = NULL;
    attach_to(p);
}

alglib::integer_1d_array::integer_1d_array(const char *s)  
{
    std::vector<const char*> svec;
    size_t i;
    char *p = filter_spaces(s);
    try
    {
        str_vector_create(p, true, &svec);
        allocate_own((ae_int_t)(svec.size()), alglib_impl::DT_INT);
        for(i=0; i<svec.size(); i++)
            operator()((ae_int_t)i) = parse_int_delim(svec[i],",]");
        alglib_impl::ae_free(p);
    }
    catch(...)
    {
        alglib_impl::ae_free(p);
        throw;
    }
}

alglib::integer_1d_array::~integer_1d_array() 
{
}

const alglib::ae_int_t& alglib::integer_1d_array::operator()(ae_int_t i) const
{
    return p_vec->ptr.p_int[i];
}

alglib::ae_int_t& alglib::integer_1d_array::operator()(ae_int_t i)
{
    return p_vec->ptr.p_int[i];
}

const alglib::ae_int_t& alglib::integer_1d_array::operator[](ae_int_t i) const
{
    return p_vec->ptr.p_int[i];
}

alglib::ae_int_t& alglib::integer_1d_array::operator[](ae_int_t i)
{
    return p_vec->ptr.p_int[i];
}

void alglib::integer_1d_array::setcontent(ae_int_t iLen, const ae_int_t *pContent )
{
    ae_int_t i;
    setlength(iLen);
    for(i=0; i<iLen; i++)
        p_vec->ptr.p_int[i] = pContent[i];
}

alglib::ae_int_t* alglib::integer_1d_array::getcontent()
{
    return p_vec->ptr.p_int;
}

const alglib::ae_int_t* alglib::integer_1d_array::getcontent() const
{
    return p_vec->ptr.p_int;
}

std::string alglib::integer_1d_array::tostring() const 
{
    if( length()==0 )
        return "[]";
    return arraytostring(&operator()(0), length());
}

alglib::real_1d_array::real_1d_array()  
{
    allocate_own(0, alglib_impl::DT_REAL);
}

alglib::real_1d_array::real_1d_array(alglib_impl::ae_vector *p)  
{
    p_vec = NULL;
    attach_to(p);
}

alglib::real_1d_array::real_1d_array(const char *s)  
{
    std::vector<const char*> svec;
    size_t i;
    char *p = filter_spaces(s);
    try
    {
        str_vector_create(p, true, &svec);
        allocate_own((ae_int_t)(svec.size()), alglib_impl::DT_REAL);
        for(i=0; i<svec.size(); i++)
            operator()((ae_int_t)i) = parse_real_delim(svec[i],",]");
        alglib_impl::ae_free(p);
    }
    catch(...)
    {
        alglib_impl::ae_free(p);
        throw;
    }
}

alglib::real_1d_array::~real_1d_array() 
{
}

const double& alglib::real_1d_array::operator()(ae_int_t i) const
{
    return p_vec->ptr.p_double[i];
}

double& alglib::real_1d_array::operator()(ae_int_t i)
{
    return p_vec->ptr.p_double[i];
}

const double& alglib::real_1d_array::operator[](ae_int_t i) const
{
    return p_vec->ptr.p_double[i];
}

double& alglib::real_1d_array::operator[](ae_int_t i)
{
    return p_vec->ptr.p_double[i];
}

void alglib::real_1d_array::setcontent(ae_int_t iLen, const double *pContent )
{
    ae_int_t i;
    setlength(iLen);
    for(i=0; i<iLen; i++)
        p_vec->ptr.p_double[i] = pContent[i];
}

double* alglib::real_1d_array::getcontent()
{
    return p_vec->ptr.p_double;
}

const double* alglib::real_1d_array::getcontent() const
{
    return p_vec->ptr.p_double;
}

std::string alglib::real_1d_array::tostring(int dps) const 
{
    if( length()==0 )
        return "[]";
    return arraytostring(&operator()(0), length(), dps);
}

alglib::complex_1d_array::complex_1d_array()  
{
    allocate_own(0, alglib_impl::DT_COMPLEX);
}

alglib::complex_1d_array::complex_1d_array(alglib_impl::ae_vector *p)  
{
    p_vec = NULL;
    attach_to(p);
}

alglib::complex_1d_array::complex_1d_array(const char *s)  
{
    std::vector<const char*> svec;
    size_t i;
    char *p = filter_spaces(s);
    try
    {
        str_vector_create(p, true, &svec);
        allocate_own((ae_int_t)(svec.size()), alglib_impl::DT_COMPLEX);
        for(i=0; i<svec.size(); i++)
            operator()((ae_int_t)i) = parse_complex_delim(svec[i],",]");
        alglib_impl::ae_free(p);
    }
    catch(...)
    {
        alglib_impl::ae_free(p);
        throw;
    }
}

alglib::complex_1d_array::~complex_1d_array() 
{
}

const alglib::complex& alglib::complex_1d_array::operator()(ae_int_t i) const
{
    return *((const alglib::complex*)(p_vec->ptr.p_complex+i));
}

alglib::complex& alglib::complex_1d_array::operator()(ae_int_t i)
{
    return *((alglib::complex*)(p_vec->ptr.p_complex+i));
}

const alglib::complex& alglib::complex_1d_array::operator[](ae_int_t i) const
{
    return *((const alglib::complex*)(p_vec->ptr.p_complex+i));
}

alglib::complex& alglib::complex_1d_array::operator[](ae_int_t i)
{
    return *((alglib::complex*)(p_vec->ptr.p_complex+i));
}

void alglib::complex_1d_array::setcontent(ae_int_t iLen, const alglib::complex *pContent )
{
    ae_int_t i;
    setlength(iLen);
    for(i=0; i<iLen; i++)
    {
        p_vec->ptr.p_complex[i].x = pContent[i].x;
        p_vec->ptr.p_complex[i].y = pContent[i].y;
    }
}

 alglib::complex* alglib::complex_1d_array::getcontent()
{
    return (alglib::complex*)p_vec->ptr.p_complex;
}

const alglib::complex* alglib::complex_1d_array::getcontent() const
{
    return (const alglib::complex*)p_vec->ptr.p_complex;
}

std::string alglib::complex_1d_array::tostring(int dps) const 
{
    if( length()==0 )
        return "[]";
    return arraytostring(&operator()(0), length(), dps);
}

alglib::ae_matrix_wrapper::ae_matrix_wrapper()
{
    p_mat = NULL;
}

alglib::ae_matrix_wrapper::~ae_matrix_wrapper()
{
    if( p_mat==&mat )
        ae_matrix_clear(p_mat);
}

alglib::ae_matrix_wrapper::ae_matrix_wrapper(const alglib::ae_matrix_wrapper &rhs)
{
    if( rhs.p_mat!=NULL )
    {
        p_mat = &mat;
        if( !ae_matrix_init_copy(p_mat, rhs.p_mat, NULL, ae_false) )
            throw alglib::ap_error("ALGLIB: malloc error!");
    }
    else
        p_mat = NULL;
}

const alglib::ae_matrix_wrapper& alglib::ae_matrix_wrapper::operator=(const alglib::ae_matrix_wrapper &rhs)
{
    if( this==&rhs )
        return *this;
    if( p_mat==&mat )
        ae_matrix_clear(p_mat);
    if( rhs.p_mat!=NULL )
    {
        p_mat = &mat;
        if( !ae_matrix_init_copy(p_mat, rhs.p_mat, NULL, ae_false) )
            throw alglib::ap_error("ALGLIB: malloc error!");
    }
    else
        p_mat = NULL;
    return *this;
}

void alglib::ae_matrix_wrapper::setlength(ae_int_t rows, ae_int_t cols)
{
    if( p_mat==NULL )
        throw alglib::ap_error("ALGLIB: setlength() error, p_mat==NULL (array was not correctly initialized)");
    if( p_mat!=&mat )
        throw alglib::ap_error("ALGLIB: setlength() error, p_mat!=&mat (attempt to resize frozen array)");
    if( !ae_matrix_set_length(p_mat, rows, cols, NULL) )
        throw alglib::ap_error("ALGLIB: malloc error");
}

alglib::ae_int_t alglib::ae_matrix_wrapper::rows() const
{
    if( p_mat==NULL )
        return 0;
    return p_mat->rows;
}

alglib::ae_int_t alglib::ae_matrix_wrapper::cols() const
{
    if( p_mat==NULL )
        return 0;
    return p_mat->cols;
}

bool alglib::ae_matrix_wrapper::isempty() const
{
    return rows()==0 || cols()==0;
}

alglib::ae_int_t alglib::ae_matrix_wrapper::getstride() const
{
    if( p_mat==NULL )
        return 0;
    return p_mat->stride;
}

void alglib::ae_matrix_wrapper::attach_to(alglib_impl::ae_matrix *ptr)
{
    if( ptr==&mat )
        throw alglib::ap_error("ALGLIB: attempt to attach matrix to itself");
    if( p_mat==&mat )
        ae_matrix_clear(p_mat);
    p_mat = ptr;
}

void alglib::ae_matrix_wrapper::allocate_own(ae_int_t rows, ae_int_t cols, alglib_impl::ae_datatype datatype)
{
    if( p_mat==&mat )
        ae_matrix_clear(p_mat);
    p_mat = &mat;
    if( !ae_matrix_init(p_mat, rows, cols, datatype, NULL, false) )
        throw alglib::ap_error("ALGLIB: malloc error");
}

const alglib_impl::ae_matrix* alglib::ae_matrix_wrapper::c_ptr() const
{
    return p_mat;
}

alglib_impl::ae_matrix* alglib::ae_matrix_wrapper::c_ptr()
{
    return p_mat;
}

alglib::boolean_2d_array::boolean_2d_array()  
{
    allocate_own(0, 0, alglib_impl::DT_BOOL);
}

alglib::boolean_2d_array::boolean_2d_array(alglib_impl::ae_matrix *p)  
{
    p_mat = NULL;
    attach_to(p);
}

alglib::boolean_2d_array::boolean_2d_array(const char *s)  
{
    std::vector< std::vector<const char*> > smat;
    size_t i, j;
    char *p = filter_spaces(s);
    try
    {
        str_matrix_create(p, &smat);
        if( smat.size()!=0 )
        {
            allocate_own((ae_int_t)(smat.size()), (ae_int_t)(smat[0].size()), alglib_impl::DT_BOOL);
            for(i=0; i<smat.size(); i++)
                for(j=0; j<smat[0].size(); j++)
                    operator()((ae_int_t)i,(ae_int_t)j) = parse_bool_delim(smat[i][j],",]");
        }
        else
            allocate_own(0, 0, alglib_impl::DT_BOOL);
        alglib_impl::ae_free(p);
    }
    catch(...)
    {
        alglib_impl::ae_free(p);
        throw;
    }
}

alglib::boolean_2d_array::~boolean_2d_array() 
{
}

const ae_bool& alglib::boolean_2d_array::operator()(ae_int_t i, ae_int_t j) const
{
    return p_mat->ptr.pp_bool[i][j];
}

ae_bool& alglib::boolean_2d_array::operator()(ae_int_t i, ae_int_t j)
{
    return p_mat->ptr.pp_bool[i][j];
}

const ae_bool* alglib::boolean_2d_array::operator[](ae_int_t i) const
{
    return p_mat->ptr.pp_bool[i];
}

ae_bool* alglib::boolean_2d_array::operator[](ae_int_t i)
{
    return p_mat->ptr.pp_bool[i];
}

void alglib::boolean_2d_array::setcontent(ae_int_t irows, ae_int_t icols, const bool *pContent )
{
    ae_int_t i, j;
    setlength(irows, icols);
    for(i=0; i<irows; i++)
        for(j=0; j<icols; j++)
            p_mat->ptr.pp_bool[i][j] = pContent[i*icols+j];
}

std::string alglib::boolean_2d_array::tostring() const 
{
    std::string result;
    ae_int_t i;
    if( isempty() )
        return "[[]]";
    result = "[";
    for(i=0; i<rows(); i++)
    {
        if( i!=0 )
            result += ",";
        result += arraytostring(&operator()(i,0), cols());
    }
    result += "]";
    return result;
}

alglib::integer_2d_array::integer_2d_array()  
{
    allocate_own(0, 0, alglib_impl::DT_INT);
}

alglib::integer_2d_array::integer_2d_array(alglib_impl::ae_matrix *p)  
{
    p_mat = NULL;
    attach_to(p);
}

alglib::integer_2d_array::integer_2d_array(const char *s)  
{
    std::vector< std::vector<const char*> > smat;
    size_t i, j;
    char *p = filter_spaces(s);
    try
    {
        str_matrix_create(p, &smat);
        if( smat.size()!=0 )
        {
            allocate_own((ae_int_t)(smat.size()), (ae_int_t)(smat[0].size()), alglib_impl::DT_INT);
            for(i=0; i<smat.size(); i++)
                for(j=0; j<smat[0].size(); j++)
                    operator()((ae_int_t)i,(ae_int_t)j) = parse_int_delim(smat[i][j],",]");
        }
        else
            allocate_own(0, 0, alglib_impl::DT_INT);
        alglib_impl::ae_free(p);
    }
    catch(...)
    {
        alglib_impl::ae_free(p);
        throw;
    }
}

alglib::integer_2d_array::~integer_2d_array() 
{
}

const alglib::ae_int_t& alglib::integer_2d_array::operator()(ae_int_t i, ae_int_t j) const
{
    return p_mat->ptr.pp_int[i][j];
}

alglib::ae_int_t& alglib::integer_2d_array::operator()(ae_int_t i, ae_int_t j)
{
    return p_mat->ptr.pp_int[i][j];
}

const alglib::ae_int_t* alglib::integer_2d_array::operator[](ae_int_t i) const
{
    return p_mat->ptr.pp_int[i];
}

alglib::ae_int_t* alglib::integer_2d_array::operator[](ae_int_t i)
{
    return p_mat->ptr.pp_int[i];
}

void alglib::integer_2d_array::setcontent(ae_int_t irows, ae_int_t icols, const ae_int_t *pContent )
{
    ae_int_t i, j;
    setlength(irows, icols);
    for(i=0; i<irows; i++)
        for(j=0; j<icols; j++)
            p_mat->ptr.pp_int[i][j] = pContent[i*icols+j];
}

std::string alglib::integer_2d_array::tostring() const 
{
    std::string result;
    ae_int_t i;
    if( isempty() )
        return "[[]]";
    result = "[";
    for(i=0; i<rows(); i++)
    {
        if( i!=0 )
            result += ",";
        result += arraytostring(&operator()(i,0), cols());
    }
    result += "]";
    return result;
}

alglib::real_2d_array::real_2d_array()  
{
    allocate_own(0, 0, alglib_impl::DT_REAL);
}

alglib::real_2d_array::real_2d_array(alglib_impl::ae_matrix *p)
{
    p_mat = NULL;
    attach_to(p);
}

alglib::real_2d_array::real_2d_array(const char *s)  
{
    std::vector< std::vector<const char*> > smat;
    size_t i, j;
    char *p = filter_spaces(s);
    try
    {
        str_matrix_create(p, &smat);
        if( smat.size()!=0 )
        {
            allocate_own((ae_int_t)(smat.size()), (ae_int_t)(smat[0].size()), alglib_impl::DT_REAL);
            for(i=0; i<smat.size(); i++)
                for(j=0; j<smat[0].size(); j++)
                    operator()((ae_int_t)i,(ae_int_t)j) = parse_real_delim(smat[i][j],",]");
        }
        else
            allocate_own(0, 0, alglib_impl::DT_REAL);
        alglib_impl::ae_free(p);
    }
    catch(...)
    {
        alglib_impl::ae_free(p);
        throw;
    }
}

alglib::real_2d_array::~real_2d_array() 
{
}

const double& alglib::real_2d_array::operator()(ae_int_t i, ae_int_t j) const
{
    return p_mat->ptr.pp_double[i][j];
}

double& alglib::real_2d_array::operator()(ae_int_t i, ae_int_t j)
{
    return p_mat->ptr.pp_double[i][j];
}

const double* alglib::real_2d_array::operator[](ae_int_t i) const
{
    return p_mat->ptr.pp_double[i];
}

double* alglib::real_2d_array::operator[](ae_int_t i)
{
    return p_mat->ptr.pp_double[i];
}

void alglib::real_2d_array::setcontent(ae_int_t irows, ae_int_t icols, const double *pContent )
{
    ae_int_t i, j;
    setlength(irows, icols);
    for(i=0; i<irows; i++)
        for(j=0; j<icols; j++)
            p_mat->ptr.pp_double[i][j] = pContent[i*icols+j];
}

std::string alglib::real_2d_array::tostring(int dps) const 
{
    std::string result;
    ae_int_t i;
    if( isempty() )
        return "[[]]";
    result = "[";
    for(i=0; i<rows(); i++)
    {
        if( i!=0 )
            result += ",";
        result += arraytostring(&operator()(i,0), cols(), dps);
    }
    result += "]";
    return result;
}

alglib::complex_2d_array::complex_2d_array()  
{
    allocate_own(0, 0, alglib_impl::DT_COMPLEX);
}

alglib::complex_2d_array::complex_2d_array(alglib_impl::ae_matrix *p)  
{
    p_mat = NULL;
    attach_to(p);
}

alglib::complex_2d_array::complex_2d_array(const char *s)  
{
    std::vector< std::vector<const char*> > smat;
    size_t i, j;
    char *p = filter_spaces(s);
    try
    {
        str_matrix_create(p, &smat);
        if( smat.size()!=0 )
        {
            allocate_own((ae_int_t)(smat.size()), (ae_int_t)(smat[0].size()), alglib_impl::DT_COMPLEX);
            for(i=0; i<smat.size(); i++)
                for(j=0; j<smat[0].size(); j++)
                    operator()((ae_int_t)i,(ae_int_t)j) = parse_complex_delim(smat[i][j],",]");
        }
        else
            allocate_own(0, 0, alglib_impl::DT_COMPLEX);
        alglib_impl::ae_free(p);
    }
    catch(...)
    {
        alglib_impl::ae_free(p);
        throw;
    }
}

alglib::complex_2d_array::~complex_2d_array() 
{
}

const alglib::complex& alglib::complex_2d_array::operator()(ae_int_t i, ae_int_t j) const
{
    return *((const alglib::complex*)(p_mat->ptr.pp_complex[i]+j));
}

alglib::complex& alglib::complex_2d_array::operator()(ae_int_t i, ae_int_t j)
{
    return *((alglib::complex*)(p_mat->ptr.pp_complex[i]+j));
}

const alglib::complex* alglib::complex_2d_array::operator[](ae_int_t i) const
{
    return (const alglib::complex*)(p_mat->ptr.pp_complex[i]);
}

alglib::complex* alglib::complex_2d_array::operator[](ae_int_t i)
{
    return (alglib::complex*)(p_mat->ptr.pp_complex[i]);
}

void alglib::complex_2d_array::setcontent(ae_int_t irows, ae_int_t icols, const alglib::complex *pContent )
{
    ae_int_t i, j;
    setlength(irows, icols);
    for(i=0; i<irows; i++)
        for(j=0; j<icols; j++)
        {
            p_mat->ptr.pp_complex[i][j].x = pContent[i*icols+j].x;
            p_mat->ptr.pp_complex[i][j].y = pContent[i*icols+j].y;
        }
}

std::string alglib::complex_2d_array::tostring(int dps) const 
{
    std::string result;
    ae_int_t i;
    if( isempty() )
        return "[[]]";
    result = "[";
    for(i=0; i<rows(); i++)
    {
        if( i!=0 )
            result += ",";
        result += arraytostring(&operator()(i,0), cols(), dps);
    }
    result += "]";
    return result;
}


/********************************************************************
Internal functions
********************************************************************/
double alglib::get_aenv_nan()
{
    double r;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    r = _alglib_env_state.v_nan;
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return r;
}

double alglib::get_aenv_posinf()
{
    double r;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    r = _alglib_env_state.v_posinf;
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return r;
}

double alglib::get_aenv_neginf()
{
    double r;
    alglib_impl::ae_state _alglib_env_state;
    alglib_impl::ae_state_init(&_alglib_env_state);
    r = _alglib_env_state.v_neginf;
    alglib_impl::ae_state_clear(&_alglib_env_state);
    return r;
}

alglib::ae_int_t alglib::my_stricmp(const char *s1, const char *s2)
{
    int c1, c2;
    
    //
    // handle special cases
    //
    if(s1==NULL && s2!=NULL)
        return -1;
    if(s1!=NULL && s2==NULL)
        return +1;
    if(s1==NULL && s2==NULL)
        return 0;

    //
    // compare
    //
    for (;;)
    {
        c1 = *s1;
        c2 = *s2;
        s1++;
        s2++;
        if( c1==0 )
            return c2==0 ? 0 : -1;
        if( c2==0 )
            return c1==0 ? 0 : +1;
        c1 = tolower(c1);
        c2 = tolower(c2);
        if( c1<c2 )
            return -1;
        if( c1>c2 )
            return +1;
    }
}

char* alglib::filter_spaces(const char *s)
{
    size_t i, j, n;
    char *r;
    char *r0;
    n = strlen(s);
    r = (char*)alglib_impl::ae_malloc(n+1, NULL);
    if( r==NULL )
        throw ap_error("malloc error");
    for(i=0,j=0,r0=r; i<=n; i++,s++)
        if( !isspace(*s) )
        {
            *r0 = *s;
            r0++;
        }
    return r;
}

void alglib::str_vector_create(const char *src, bool match_head_only, std::vector<const char*> *p_vec)
{
    //
    // parse beginning of the string.
    // try to handle "[]" string
    //
    p_vec->clear();
    if( *src!='[' )
        throw alglib::ap_error("Incorrect initializer for vector");
    src++;
    if( *src==']' )
        return;
    p_vec->push_back(src);
    for(;;)
    {
        if( *src==0 )
            throw alglib::ap_error("Incorrect initializer for vector");
        if( *src==']' )
        {
            if( src[1]==0 || !match_head_only)
                return;
            throw alglib::ap_error("Incorrect initializer for vector");
        }
        if( *src==',' )
        {
            p_vec->push_back(src+1);
            src++;
            continue;
        }
        src++;
    }
}

void alglib::str_matrix_create(const char *src, std::vector< std::vector<const char*> > *p_mat)
{
    p_mat->clear();
    
    //
    // Try to handle "[[]]" string
    //
    if( strcmp(src, "[[]]")==0 )
        return;

    //
    // Parse non-empty string
    //
    if( *src!='[' )
        throw alglib::ap_error("Incorrect initializer for matrix");
    src++;
    for(;;)
    {
        p_mat->push_back(std::vector<const char*>());
        str_vector_create(src, false, &p_mat->back());
        if( p_mat->back().size()==0 || p_mat->back().size()!=(*p_mat)[0].size() )
            throw alglib::ap_error("Incorrect initializer for matrix");
        src = strchr(src, ']');
        if( src==NULL )
            throw alglib::ap_error("Incorrect initializer for matrix");
        src++;
        if( *src==',' )
        {
            src++;
            continue;
        }
        if( *src==']' )
            break;
        throw alglib::ap_error("Incorrect initializer for matrix");
    }
    src++;
    if( *src!=0 )
        throw alglib::ap_error("Incorrect initializer for matrix");
}

ae_bool alglib::parse_bool_delim(const char *s, const char *delim)
{
    const char *p;
    char buf[8];
    
    // try to parse false
    p = "false";
    memset(buf, 0, sizeof(buf));
    strncpy(buf, s, strlen(p));
    if( my_stricmp(buf, p)==0 )
    {
        if( s[strlen(p)]==0 || strchr(delim,s[strlen(p)])==NULL )
            throw alglib::ap_error("Cannot parse value");
        return ae_false;
    }

    // try to parse true
    p = "true";
    memset(buf, 0, sizeof(buf));
    strncpy(buf, s, strlen(p));
    if( my_stricmp(buf, p)==0 )
    {
        if( s[strlen(p)]==0 || strchr(delim,s[strlen(p)])==NULL )
            throw alglib::ap_error("Cannot parse value");
        return ae_true;
    }

    // error
    throw alglib::ap_error("Cannot parse value");
}

alglib::ae_int_t alglib::parse_int_delim(const char *s, const char *delim)
{
    const char *p;
    long long_val;
    volatile ae_int_t ae_val;
    
    p = s;

    //
    // check string structure:
    // * leading sign
    // * at least one digit
    // * delimiter
    //
    if( *s=='-' || *s=='+' )
        s++;
    if( *s==0 || strchr("1234567890",*s)==NULL)
        throw alglib::ap_error("Cannot parse value");
    while( *s!=0 && strchr("1234567890",*s)!=NULL )
        s++;
    if( *s==0 || strchr(delim,*s)==NULL )
        throw alglib::ap_error("Cannot parse value");

    // convert and ensure that value fits into ae_int_t
    s = p;
    long_val = atol(s);
    ae_val = long_val;
    if( ae_val!=long_val )
        throw alglib::ap_error("Cannot parse value");
    return ae_val;
}

bool alglib::_parse_real_delim(const char *s, const char *delim, double *result, const char **new_s)
{
    const char *p;
    char *t;
    bool has_digits;
    char buf[64];
    int  isign;
    lconv *loc;

    p = s;
    
    //
    // check string structure and decide what to do
    //
    isign = 1;
    if( *s=='-' || *s=='+' )
    {
        isign = *s=='-' ? -1 : +1;
        s++;
    }
    memset(buf, 0, sizeof(buf));
    strncpy(buf, s, 3);
    if( my_stricmp(buf,"nan")!=0 && my_stricmp(buf,"inf")!=0 )
    {
        //
        // [sign] [ddd] [.] [ddd] [e|E[sign]ddd]
        //
        has_digits = false;
        if( *s!=0 && strchr("1234567890",*s)!=NULL )
        {
            has_digits = true;
            while( *s!=0 && strchr("1234567890",*s)!=NULL )
                s++;
        }
        if( *s=='.' )
            s++;
        if( *s!=0 && strchr("1234567890",*s)!=NULL )
        {
            has_digits = true;
            while( *s!=0 && strchr("1234567890",*s)!=NULL )
                s++;
        }
        if (!has_digits )
            return false;
        if( *s=='e' || *s=='E' )
        {
            s++;
            if( *s=='-' || *s=='+' )
                s++;
            if( *s==0 || strchr("1234567890",*s)==NULL )
                return false;
            while( *s!=0 && strchr("1234567890",*s)!=NULL )
                s++;
        }   
        if( *s==0 || strchr(delim,*s)==NULL )
            return false;
        *new_s = s;

        //
        // finite value conversion
        //
        if( *new_s-p>=(int)sizeof(buf) )
            return false;
        strncpy(buf, p, (size_t)(*new_s-p));
        buf[*new_s-p] = 0;
        loc = localeconv();
        t = strchr(buf,'.');
        if( t!=NULL )
            *t = *loc->decimal_point;
        *result = atof(buf);
        return true;
    }
    else
    {
        //
        // check delimiter and update *new_s
        //
        s += 3;
        if( *s==0 || strchr(delim,*s)==NULL )
            return false;
        *new_s = s;

        //
        // NAN, INF conversion
        //
        if( my_stricmp(buf,"nan")==0 )
            *result = fp_nan;
        if( my_stricmp(buf,"inf")==0 )
            *result = isign>0 ? fp_posinf : fp_neginf;
        return true;
    }
}

double alglib::parse_real_delim(const char *s, const char *delim)
{
    double result;
    const char *new_s;
    if( !_parse_real_delim(s, delim, &result, &new_s) )
        throw alglib::ap_error("Cannot parse value");
    return result;
}

alglib::complex alglib::parse_complex_delim(const char *s, const char *delim)
{
    double d_result;
    const char *new_s;
    alglib::complex c_result;
    
    // parse as real value
    if( _parse_real_delim(s, delim, &d_result, &new_s) )
        return d_result;

    // parse as "a+bi" or "a-bi"
    if( _parse_real_delim(s, "+-", &c_result.x, &new_s) )
    {
        s = new_s;
        if( !_parse_real_delim(s, "i", &c_result.y, &new_s) )
            throw alglib::ap_error("Cannot parse value");
        s = new_s+1;
        if( *s==0 || strchr(delim,*s)==NULL )
            throw alglib::ap_error("Cannot parse value");
        return c_result;
    }
    
    // parse as complex value "bi+a" or "bi-a"
    if( _parse_real_delim(s, "i", &c_result.y, &new_s) )
    {
        s = new_s+1;
        if( *s==0 )
            throw alglib::ap_error("Cannot parse value");
        if( strchr(delim,*s)!=NULL )
        {
            c_result.x = 0;
            return c_result;
        }
        if( strchr("+-",*s)!=NULL )
        {
            if( !_parse_real_delim(s, delim, &c_result.x, &new_s) )
                throw alglib::ap_error("Cannot parse value");
            return c_result;
        }
        throw alglib::ap_error("Cannot parse value");
    }

    // error
    throw alglib::ap_error("Cannot parse value");
}

std::string alglib::arraytostring(const bool *ptr, ae_int_t n)
{
    std::string result;
    ae_int_t i;
    result = "[";
    for(i=0; i<n; i++)
    {
        if( i!=0 )
            result += ",";
        result += ptr[i] ? "true" : "false";
    }
    result += "]";
    return result;
}

std::string alglib::arraytostring(const ae_int_t *ptr, ae_int_t n)
{
    std::string result;
    ae_int_t i;
    char buf[64];
    result = "[";
    for(i=0; i<n; i++)
    {
        if( sprintf(buf, i==0 ? "%ld" : ",%ld", long(ptr[i]))>=(int)sizeof(buf) )
            throw ap_error("arraytostring(): buffer overflow");
        result += buf;
    }
    result += "]";
    return result;
}

std::string alglib::arraytostring(const double *ptr, ae_int_t n, int dps)
{
    std::string result;
    ae_int_t i;
    char buf[64];
    char mask1[64];
    char mask2[64];
    result = "[";
    if( sprintf(mask1, "%%.%df", dps)>=(int)sizeof(mask1) )
        throw ap_error("arraytostring(): buffer overflow");
    if( sprintf(mask2, ",%s", mask1)>=(int)sizeof(mask2) )
        throw ap_error("arraytostring(): buffer overflow");
    for(i=0; i<n; i++)
    {
        buf[0] = 0;
        if( fp_isfinite(ptr[i]) )
        {
            if( sprintf(buf, i==0 ? mask1 : mask2, double(ptr[i]))>=(int)sizeof(buf) )
                throw ap_error("arraytostring(): buffer overflow");
        }
        else if( fp_isnan(ptr[i]) )
            strcpy(buf, i==0 ?  "NAN" :  ",NAN");
        else if( fp_isposinf(ptr[i]) )
            strcpy(buf, i==0 ? "+INF" : ",+INF");
        else if( fp_isneginf(ptr[i]) )
            strcpy(buf, i==0 ? "-INF" : ",-INF");
        result += buf;
    }
    result += "]";
    return result;
}

std::string alglib::arraytostring(const alglib::complex *ptr, ae_int_t n, int dps)
{
    std::string result;
    ae_int_t i;
    result = "[";
    for(i=0; i<n; i++)
    {
        if( i!=0 )
            result += ",";
        result += ptr[i].tostring(dps);
    }
    result += "]";
    return result;
}


/********************************************************************
standard functions
********************************************************************/
int alglib::sign(double x)
{
    if( x>0 ) return  1;
    if( x<0 ) return -1;
    return 0;
}

double alglib::randomreal()
{
    int i1 = rand();
    int i2 = rand();
    while(i1==RAND_MAX)
        i1 =rand();
    while(i2==RAND_MAX)
        i2 =rand();
    double mx = RAND_MAX;
    return (i1+i2/mx)/mx;
}

int alglib::randominteger(int maxv)
{  return rand()%maxv; }

int alglib::round(double x)
{ return int(floor(x+0.5)); }

int alglib::trunc(double x)
{ return int(x>0 ? floor(x) : ceil(x)); }

int alglib::ifloor(double x)
{ return int(floor(x)); }

int alglib::iceil(double x)
{ return int(ceil(x)); }

double alglib::pi()
{ return 3.14159265358979323846; }

double alglib::sqr(double x)
{ return x*x; }

int alglib::maxint(int m1, int m2)
{
    return m1>m2 ? m1 : m2;
}

int alglib::minint(int m1, int m2)
{
    return m1>m2 ? m2 : m1;
}

double alglib::maxreal(double m1, double m2)
{
    return m1>m2 ? m1 : m2;
}

double alglib::minreal(double m1, double m2)
{
    return m1>m2 ? m2 : m1;
}

bool alglib::fp_eq(double v1, double v2)
{
    // IEEE-strict floating point comparison
    volatile double x = v1;
    volatile double y = v2;
    return x==y;
}

bool alglib::fp_neq(double v1, double v2)
{
    // IEEE-strict floating point comparison
    return !fp_eq(v1,v2);
}

bool alglib::fp_less(double v1, double v2)
{
    // IEEE-strict floating point comparison
    volatile double x = v1;
    volatile double y = v2;
    return x<y;
}

bool alglib::fp_less_eq(double v1, double v2)
{
    // IEEE-strict floating point comparison
    volatile double x = v1;
    volatile double y = v2;
    return x<=y;
}

bool alglib::fp_greater(double v1, double v2)
{
    // IEEE-strict floating point comparison
    volatile double x = v1;
    volatile double y = v2;
    return x>y;
}

bool alglib::fp_greater_eq(double v1, double v2)
{
    // IEEE-strict floating point comparison
    volatile double x = v1;
    volatile double y = v2;
    return x>=y;
}

bool alglib::fp_isnan(double x)
{
    return alglib_impl::ae_isnan_stateless(x,endianness);
}

bool alglib::fp_isposinf(double x)
{
    return alglib_impl::ae_isposinf_stateless(x,endianness);
}

bool alglib::fp_isneginf(double x)
{
    return alglib_impl::ae_isneginf_stateless(x,endianness);
}

bool alglib::fp_isinf(double x)
{
    return alglib_impl::ae_isinf_stateless(x,endianness);
}

bool alglib::fp_isfinite(double x)
{
    return alglib_impl::ae_isfinite_stateless(x,endianness);
}

/********************************************************************
Dataset functions
********************************************************************/
/*bool alglib::readstrings(std::string file, std::list<std::string> *pOutput)
{
    return readstrings(file, pOutput, "");
}

bool alglib::readstrings(std::string file, std::list<std::string> *pOutput, std::string comment)
{
    std::string cmd, s;
    FILE *f;
    char buf[32768];
    char *str;

    f = fopen(file.c_str(), "rb");
    if( !f )
        return false;
    s = "";
    pOutput->clear();
    while(str=fgets(buf, sizeof(buf), f))
    {
        // TODO: read file by small chunks, combine in one large string
        if( strlen(str)==0 )
            continue;
            
        //
        // trim trailing newline chars
        //
        char *eos = str+strlen(str)-1;
        if( *eos=='\n' )
        {
            *eos = 0;
            eos--;
        }
        if( *eos=='\r' )
        {
            *eos = 0;
            eos--;
        }
        s = str;

        //
        // skip comments
        //
        if( comment.length()>0 )
            if( strncmp(s.c_str(), comment.c_str(), comment.length())==0 )
            {
                s = "";
                continue;
            }

        //
        // read data
        //
        if( s.length()<1 )
        {
            fclose(f);
            throw alglib::ap_error("internal error in read_strings");
        }
        pOutput->push_back(s);
    }
    fclose(f);
    return true;
}

void alglib::explodestring(std::string s, char sep, std::vector<std::string> *pOutput)
{
    std::string tmp;
    int i;
    tmp = "";
    pOutput->clear();
    for(i=0; i<s.length(); i++)
    {
        if( s[i]!=sep )
        {
            tmp += s[i];
            continue;
        }
        //if( tmp.length()!=0 )
        pOutput->push_back(tmp);
        tmp = "";
    }
    if( tmp.length()!=0 )
        pOutput->push_back(tmp);
}

std::string alglib::strtolower(const std::string &s)
{
    std::string r = s;
    for(int i=0; i<r.length(); i++)
        r[i] = tolower(r[i]);
    return r;
}

std::string alglib::xtrim(std::string s)
{
    char *pstr = (char*)malloc(s.length()+1);
    char *p2 = pstr;
    if( pstr==NULL )
        throw "xalloc in xtrim()";
    try
    {
        bool bws;
        int i;

        //
        // special cases:
        // * zero length string
        // * string includes only spaces
        //
        if( s.length()==0 )
        {
            free(pstr);
            return "";
        }
        bws = true;
        for(i=0; i<s.length(); i++)
            if( s[i]!=' ' )
                bws = false;
        if( bws )
        {
            free(pstr);
            return "";
        }

        //
        // merge internal spaces
        //
        bws = false;
        for(i=0; i<s.length(); i++)
        {
            if( s[i]==' ' && bws )
                continue;
            if( s[i]==' ' )
            {
                *p2 = ' ';
                p2++;
                bws = true;
                continue;
            }
            *p2 = s[i];
            bws = false;
            p2++;
        }
        *p2 = 0;

        //
        // trim leading/trailing spaces.
        // we expect at least one non-space character in the string
        //
        p2--;
        while(*p2==' ')
        {
            *p2 = 0;
            p2--;
        }
        p2 = pstr;
        while((*p2)==' ')
            p2++;

        //
        // result
        //
        std::string r = p2;
        free(pstr);
        return r;
    }
    catch(...)
    {
        free(pstr);
        throw "unknown exception in xtrim()";
    }
}

bool alglib::opendataset(std::string file, dataset *pdataset)
{
    std::list<std::string> Lines;
    std::vector<std::string> Values, RowsArr, ColsArr, VarsArr, HeadArr;
    std::list<std::string>::iterator i;
    std::string s;
    int TrnFirst, TrnLast, ValFirst, ValLast, TstFirst, TstLast, LinesRead, j;

    //
    // Read data
    //
    if( pdataset==NULL )
        return false;
    if( !readstrings(file, &Lines, "//") )
        return false;
    i = Lines.begin();
    *pdataset = dataset();

    //
    // Read header
    //
    if( i==Lines.end() )
        return false;
    s = alglib::xtrim(*i);
    alglib::explodestring(s, '#', &HeadArr);
    if( HeadArr.size()!=2 )
        return false;

    //
    // Rows info
    //
    alglib::explodestring(alglib::xtrim(HeadArr[0]), ' ', &RowsArr);
    if( RowsArr.size()==0 || RowsArr.size()>3 )
        return false;
    if( RowsArr.size()==1 )
    {
        pdataset->totalsize = atol(RowsArr[0].c_str());
        pdataset->trnsize = pdataset->totalsize;
    }
    if( RowsArr.size()==2 )
    {
        pdataset->trnsize = atol(RowsArr[0].c_str());
        pdataset->tstsize = atol(RowsArr[1].c_str());
        pdataset->totalsize = pdataset->trnsize + pdataset->tstsize;
    }
    if( RowsArr.size()==3 )
    {
        pdataset->trnsize = atol(RowsArr[0].c_str());
        pdataset->valsize = atol(RowsArr[1].c_str());
        pdataset->tstsize = atol(RowsArr[2].c_str());
        pdataset->totalsize = pdataset->trnsize + pdataset->valsize + pdataset->tstsize;
    }
    if( pdataset->totalsize<=0 || pdataset->trnsize<0 || pdataset->valsize<0 || pdataset->tstsize<0 )
        return false;
    TrnFirst = 0;
    TrnLast = TrnFirst + pdataset->trnsize;
    ValFirst = TrnLast;
    ValLast = ValFirst + pdataset->valsize;
    TstFirst = ValLast;
    TstLast = TstFirst + pdataset->tstsize;
                
    //
    // columns
    //
    alglib::explodestring(alglib::xtrim(HeadArr[1]), ' ', &ColsArr);
    if( ColsArr.size()!=1 && ColsArr.size()!=4 )
        return false;
    if( ColsArr.size()==1 )
    {
        pdataset->nin = atoi(ColsArr[0].c_str());
        if( pdataset->nin<=0 )
            return false;
    }
    if( ColsArr.size()==4 )
    {
        if( alglib::strtolower(ColsArr[0])!="reg" && alglib::strtolower(ColsArr[0])!="cls" )
            return false;
        if( ColsArr[2]!="=>" )
            return false;
        pdataset->nin = atol(ColsArr[1].c_str());
        if( pdataset->nin<1 )
            return false;
        if( alglib::strtolower(ColsArr[0])=="reg" )
        {
            pdataset->nclasses = 0;
            pdataset->nout = atol(ColsArr[3].c_str());
            if( pdataset->nout<1 )
                return false;
        }
        else
        {
            pdataset->nclasses = atol(ColsArr[3].c_str());
            pdataset->nout = 1;
            if( pdataset->nclasses<2 )
                return false;
        }
    }

    //
    // initialize arrays
    //
    pdataset->all.setlength(pdataset->totalsize, pdataset->nin+pdataset->nout);
    if( pdataset->trnsize>0 ) pdataset->trn.setlength(pdataset->trnsize, pdataset->nin+pdataset->nout);
    if( pdataset->valsize>0 ) pdataset->val.setlength(pdataset->valsize, pdataset->nin+pdataset->nout);
    if( pdataset->tstsize>0 ) pdataset->tst.setlength(pdataset->tstsize, pdataset->nin+pdataset->nout);

    //
    // read data
    //
    for(LinesRead=0, i++; i!=Lines.end() && LinesRead<pdataset->totalsize; i++, LinesRead++)
    {
        std::string sss = *i;
        alglib::explodestring(alglib::xtrim(*i), ' ', &VarsArr);
        if( VarsArr.size()!=pdataset->nin+pdataset->nout )
            return false;
        int tmpc = alglib::round(atof(VarsArr[pdataset->nin+pdataset->nout-1].c_str()));
        if( pdataset->nclasses>0 && (tmpc<0 || tmpc>=pdataset->nclasses) )
            return false;
        for(j=0; j<pdataset->nin+pdataset->nout; j++)
        {
            pdataset->all(LinesRead,j) = atof(VarsArr[j].c_str());
            if( LinesRead>=TrnFirst && LinesRead<TrnLast )
                pdataset->trn(LinesRead-TrnFirst,j) = atof(VarsArr[j].c_str());
            if( LinesRead>=ValFirst && LinesRead<ValLast )
                pdataset->val(LinesRead-ValFirst,j) = atof(VarsArr[j].c_str());
            if( LinesRead>=TstFirst && LinesRead<TstLast )
                pdataset->tst(LinesRead-TstFirst,j) = atof(VarsArr[j].c_str());
        }
    }
    if( LinesRead!=pdataset->totalsize )
        return false;
    return true;
}*/

/*
previous variant
bool alglib::opendataset(std::string file, dataset *pdataset)
{
    std::list<std::string> Lines;
    std::vector<std::string> Values;
    std::list<std::string>::iterator i;
    int nCol, nRow, nSplitted;
    int nColumns, nRows;

    //
    // Read data
    //
    if( pdataset==NULL )
        return false;
    if( !readstrings(file, &Lines, "//") )
        return false;
    i = Lines.begin();
    *pdataset = dataset();

    //
    // Read columns info
    //
    if( i==Lines.end() )
        return false;
    if( sscanf(i->c_str(), " columns = %d %d ", &pdataset->nin, &pdataset->nout)!=2 )
        return false;
    if( pdataset->nin<=0 || pdataset->nout==0 || pdataset->nout==-1)
        return false;
    if( pdataset->nout<0 )
    {
        pdataset->nclasses = -pdataset->nout;
        pdataset->nout = 1;
        pdataset->iscls = true;
    }
    else
    {
        pdataset->isreg = true;
    }
    nColumns = pdataset->nin+pdataset->nout;
    i++;

    //
    // Read rows info
    //
    if( i==Lines.end() )
        return false;
    if( sscanf(i->c_str(), " rows = %d %d %d ", &pdataset->trnsize, &pdataset->valsize, &pdataset->tstsize)!=3 )
        return false;
    if( (pdataset->trnsize<0) || (pdataset->valsize<0) || (pdataset->tstsize<0) )
        return false;
    if( (pdataset->trnsize==0) && (pdataset->valsize==0) && (pdataset->tstsize==0) )
        return false;
    nRows = pdataset->trnsize+pdataset->valsize+pdataset->tstsize;
    pdataset->size = nRows;
    if( Lines.size()!=nRows+2 )
        return false;
    i++;

    //
    // Read all cases
    //
    alglib::real_2d_array &arr = pdataset->all;
    arr.setbounds(0, nRows-1, 0, nColumns-1);
    for(nRow=0; nRow<nRows; nRow++)
    {
        alglib::ap_error::make_assertion(i!=Lines.end());
        explodestring(*i, '\t', &Values);
        if( Values.size()!=nColumns )
            return false;
        for(nCol=0; nCol<nColumns; nCol++)
        {
            double v;
            if( sscanf(Values[nCol].c_str(), "%lg", &v)!=1 )
                return false;
            if( (nCol==nColumns-1) && pdataset->iscls && ((round(v)<0) || (round(v)>=pdataset->nclasses)) )
                return false;
            if( (nCol==nColumns-1) && pdataset->iscls )
                arr(nRow, nCol) = round(v);
            else
                arr(nRow, nCol) = v;
        }
        i++;
    }

    //
    // Split to training, validation and test sets
    //
    if( pdataset->trnsize>0 )
        pdataset->trn.setbounds(0, pdataset->trnsize-1, 0, nColumns-1);
    if( pdataset->valsize>0 )
        pdataset->val.setbounds(0, pdataset->valsize-1, 0, nColumns-1);
    if( pdataset->tstsize>0 )
        pdataset->tst.setbounds(0, pdataset->tstsize-1, 0, nColumns-1);
    nSplitted=0;
    for(nRow=0; nRow<=pdataset->trnsize-1; nRow++, nSplitted++)
        for(nCol=0; nCol<=nColumns-1; nCol++)
            pdataset->trn(nRow,nCol) = arr(nSplitted,nCol);
    for(nRow=0; nRow<=pdataset->valsize-1; nRow++, nSplitted++)
        for(nCol=0; nCol<=nColumns-1; nCol++)
            pdataset->val(nRow,nCol) = arr(nSplitted,nCol);
    for(nRow=0; nRow<=pdataset->tstsize-1; nRow++, nSplitted++)
        for(nCol=0; nCol<=nColumns-1; nCol++)
            pdataset->tst(nRow,nCol) = arr(nSplitted,nCol);
    return true;
}*/

alglib::ae_int_t alglib::vlen(ae_int_t n1, ae_int_t n2)
{
    return n2-n1+1;
}



/////////////////////////////////////////////////////////////////////////
//
// THIS SECTIONS CONTAINS OPTIMIZED LINEAR ALGEBRA CODE
// IT IS SHARED BETWEEN C++ AND PURE C LIBRARIES
//
/////////////////////////////////////////////////////////////////////////
namespace alglib_impl
{
static const ae_int_t alglib_simd_alignment = 16;
static const ae_int_t alglib_r_block        = 32;
static const ae_int_t alglib_c_block        = 24;
static const ae_int_t alglib_half_r_block   = alglib_r_block/2;
static const ae_int_t alglib_half_c_block   = alglib_c_block/2;
static const ae_int_t alglib_twice_r_block  = alglib_r_block*2;
static const ae_int_t alglib_twice_c_block  = alglib_c_block*2;

/********************************************************************
This subroutine calculates fast 32x32 real matrix-vector product:

    y := beta*y + alpha*A*x

using either generic C code or native optimizations (if available)

IMPORTANT:
* A must be stored in row-major order,
  stride is alglib_r_block,
  aligned on alglib_simd_alignment boundary
* X must be aligned on alglib_simd_alignment boundary
* Y may be non-aligned
********************************************************************/
void _ialglib_mv_32(const double *a, const double *x, double *y, ae_int_t stride, double alpha, double beta)
{
    ae_int_t i, k;
    const double *pa0, *pa1, *pb;

    pa0 = a;
    pa1 = a+alglib_r_block;
    pb = x;
    for(i=0; i<16; i++)
    {
        double v0 = 0, v1 = 0;
        for(k=0; k<4; k++)
        {
            v0 += pa0[0]*pb[0];
            v1 += pa1[0]*pb[0];
            v0 += pa0[1]*pb[1];
            v1 += pa1[1]*pb[1];
            v0 += pa0[2]*pb[2];
            v1 += pa1[2]*pb[2];
            v0 += pa0[3]*pb[3];
            v1 += pa1[3]*pb[3];
            v0 += pa0[4]*pb[4];
            v1 += pa1[4]*pb[4];
            v0 += pa0[5]*pb[5];
            v1 += pa1[5]*pb[5];
            v0 += pa0[6]*pb[6];
            v1 += pa1[6]*pb[6];
            v0 += pa0[7]*pb[7];
            v1 += pa1[7]*pb[7];
            pa0 += 8;
            pa1 += 8;
            pb  += 8;
        }
        y[0] = beta*y[0]+alpha*v0;
        y[stride] = beta*y[stride]+alpha*v1;

        //
        // now we've processed rows I and I+1,
        // pa0 and pa1 are pointing to rows I+1 and I+2.
        // move to I+2 and I+3.
        //
        pa0 += alglib_r_block;
        pa1 += alglib_r_block;
        pb = x;
        y+=2*stride;
    }
}


/********************************************************************
This subroutine calculates fast MxN real matrix-vector product:

    y := beta*y + alpha*A*x

using either generic C code or native optimizations (if available).
It calls _ialglib_mv_32 if both M=32 and N=32.

IMPORTANT:
* 0<=M<=alglib_r_block, 0<=N<=alglib_r_block
* A must be stored in row-major order,
  stride is alglib_r_block

ALIGNMENT REQUIREMENTS:
1. everything may be non-aligned. in such cases we call
   generic C code.
2. for better performance, each row of A should be aligned,
   X should be aligned too. Y may be non-aligned - does not matter.
********************************************************************/
void _ialglib_mv(ae_int_t m, ae_int_t n, const double *a, const double *x, double *y, ae_int_t stride, double alpha, double beta)
{
    _ialglib_mv_generic(m, n, a, x, y, stride, alpha, beta);
}

/********************************************************************
This is generic C implementation of mv.
It may work with unaligned data.
********************************************************************/
void _ialglib_mv_generic(ae_int_t m, ae_int_t n, const double *a, const double *x, double *y, ae_int_t stride, double alpha, double beta)
{
    if( m==32 && n==32 )
    {
        //
        // 32x32, may be we have something better than general implementation
        //
        _ialglib_mv_32(a, x, y, stride, alpha, beta);
    }
    else
    {
        ae_int_t i, k, m2, n8, n2, ntrail2;
        const double *pa0, *pa1, *pb;

        //
        // First M/2 rows of A are processed in pairs.
        // optimized code is used.
        //
        m2 = m/2;
        n8 = n/8;
        ntrail2 = (n-8*n8)/2;
        for(i=0; i<m2; i++)
        {
            double v0 = 0, v1 = 0;

            //
            // 'a' points to the part of the matrix which
            // is not processed yet
            //
            pb = x;
            pa0 = a;
            pa1 = a+alglib_r_block;
            a += alglib_twice_r_block;

            //
            // 8 elements per iteration
            //
            for(k=0; k<n8; k++)
            {
                v0 += pa0[0]*pb[0];
                v1 += pa1[0]*pb[0];
                v0 += pa0[1]*pb[1];
                v1 += pa1[1]*pb[1];
                v0 += pa0[2]*pb[2];
                v1 += pa1[2]*pb[2];
                v0 += pa0[3]*pb[3];
                v1 += pa1[3]*pb[3];
                v0 += pa0[4]*pb[4];
                v1 += pa1[4]*pb[4];
                v0 += pa0[5]*pb[5];
                v1 += pa1[5]*pb[5];
                v0 += pa0[6]*pb[6];
                v1 += pa1[6]*pb[6];
                v0 += pa0[7]*pb[7];
                v1 += pa1[7]*pb[7];
                pa0 += 8;
                pa1 += 8;
                pb  += 8;
            }

            //
            // 2 elements per iteration
            //
            for(k=0; k<ntrail2; k++)
            {
                v0 += pa0[0]*pb[0];
                v1 += pa1[0]*pb[0];
                v0 += pa0[1]*pb[1];
                v1 += pa1[1]*pb[1];
                pa0 += 2;
                pa1 += 2;
                pb  += 2;
            }

            //
            // last element, if needed
            //
            if( n%2!=0 )
            {
                v0 += pa0[0]*pb[0];
                v1 += pa1[0]*pb[0];
            }

            //
            // final update
            //
            y[0] = beta*y[0]+alpha*v0;
            y[stride] = beta*y[stride]+alpha*v1;

            //
            // move to the next pair of elements
            //
            y+=2*stride;
        }


        //
        // Last (odd) row is processed with less optimized code.
        //
        if( m%2!=0 )
        {
            double v0 = 0;

            //
            // 'a' points to the part of the matrix which
            // is not processed yet
            //
            pb = x;
            pa0 = a;

            //
            // 2 elements per iteration
            //
            n2 = n/2;
            for(k=0; k<n2; k++)
            {
                v0 += pa0[0]*pb[0]+pa0[1]*pb[1];
                pa0 += 2;
                pb  += 2;
            }

            //
            // last element, if needed
            //
            if( n%2!=0 )
                v0 += pa0[0]*pb[0];

            //
            // final update
            //
            y[0] = beta*y[0]+alpha*v0;
        }
    }
}

/********************************************************************
This subroutine calculates fast MxN complex matrix-vector product:

    y := beta*y + alpha*A*x

using either generic C code or native optimizations (if available).

IMPORTANT:
* 0<=M<=alglib_c_block, 0<=N<=alglib_c_block
* A must be stored in row-major order,
  stride is alglib_c_block,
* Y may be referenced by cy (pointer to ae_complex) or
  dy (pointer to double) depending on what type of output you
  wish. Pass pointer to Y as one of these parameters,
  AND SET OTHER PARAMETER TO NULL.


ALIGNMENT REQUIREMENTS:
1. everything may be non-aligned. in such cases we call
   generic C code.
2. for better performance, each row of A should be aligned,
   X should be aligned too. Y may be non-aligned - does not matter.
********************************************************************/
void _ialglib_mv_complex(ae_int_t m, ae_int_t n, const double *a, const double *x, ae_complex *cy, double *dy, ae_int_t stride, ae_complex alpha, ae_complex beta)
{
    _ialglib_mv_complex_generic(m, n, a, x, cy, dy, stride, alpha, beta);
}

/********************************************************************
This is generic C implementation of mv_complex
It may work with unaligned data.
********************************************************************/
void _ialglib_mv_complex_generic(ae_int_t m, ae_int_t n, const double *a, const double *x, ae_complex *cy, double *dy, ae_int_t stride, ae_complex alpha, ae_complex beta)
{
    ae_int_t i, j;
    const double *pa, *parow, *pb;

    parow = a;
    for(i=0; i<m; i++)
    {
        double v0 = 0, v1 = 0;
        pa = parow;
        pb = x;
        for(j=0; j<n; j++)
        {
            v0 += pa[0]*pb[0];
            v1 += pa[0]*pb[1];
            v0 -= pa[1]*pb[1];
            v1 += pa[1]*pb[0];

            pa  += 2;
            pb  += 2;
        }
        if( cy!=NULL )
        {
            double tx = (beta.x*cy->x-beta.y*cy->y)+(alpha.x*v0-alpha.y*v1);
            double ty = (beta.x*cy->y+beta.y*cy->x)+(alpha.x*v1+alpha.y*v0);
            cy->x = tx;
            cy->y = ty;
            cy+=stride;
        }
        else
        {
            double tx = (beta.x*dy[0]-beta.y*dy[1])+(alpha.x*v0-alpha.y*v1);
            double ty = (beta.x*dy[1]+beta.y*dy[0])+(alpha.x*v1+alpha.y*v0);
            dy[0] = tx;
            dy[1] = ty;
            dy += 2*stride;
        }
        parow += 2*alglib_c_block;
    }
}


/********************************************************************
This subroutine sets vector to zero
********************************************************************/
void _ialglib_vzero(ae_int_t n, double *p, ae_int_t stride)
{
    ae_int_t i;
    if( stride==1 )
    {
        for(i=0; i<n; i++,p++)
            *p = 0.0;
    }
    else
    {
        for(i=0; i<n; i++,p+=stride)
            *p = 0.0;
    }
}

/********************************************************************
This subroutine sets vector to zero
********************************************************************/
void _ialglib_vzero_complex(ae_int_t n, ae_complex *p, ae_int_t stride)
{
    ae_int_t i;
    if( stride==1 )
    {
        for(i=0; i<n; i++,p++)
        {
            p->x = 0.0;
            p->y = 0.0;
        }
    }
    else
    {
        for(i=0; i<n; i++,p+=stride)
        {
            p->x = 0.0;
            p->y = 0.0;
        }
    }
}


/********************************************************************
This subroutine copies unaligned real vector
********************************************************************/
void _ialglib_vcopy(ae_int_t n, const double *a, ae_int_t stridea, double *b, ae_int_t strideb)
{
    ae_int_t i, n2;
    if( stridea==1 && strideb==1 )
    {
        n2 = n/2;
        for(i=n2; i!=0; i--, a+=2, b+=2)
        {
            b[0] = a[0];
            b[1] = a[1];
        }
        if( n%2!=0 )
            b[0] = a[0];
    }
    else
    {
        for(i=0; i<n; i++,a+=stridea,b+=strideb)
            *b = *a;
    }
}


/********************************************************************
This subroutine copies unaligned complex vector
(passed as ae_complex*)

1. strideb is stride measured in complex numbers, not doubles
2. conj may be "N" (no conj.) or "C" (conj.)
********************************************************************/
void _ialglib_vcopy_complex(ae_int_t n, const ae_complex *a, ae_int_t stridea, double *b, ae_int_t strideb, const char *conj)
{
    ae_int_t i;

    //
    // more general case
    //
    if( conj[0]=='N' || conj[0]=='n' )
    {
        for(i=0; i<n; i++,a+=stridea,b+=2*strideb)
        {
            b[0] = a->x;
            b[1] = a->y;
        }
    }
    else
    {
        for(i=0; i<n; i++,a+=stridea,b+=2*strideb)
        {
            b[0] = a->x;
            b[1] = -a->y;
        }
    }
}


/********************************************************************
This subroutine copies unaligned complex vector (passed as double*)

1. strideb is stride measured in complex numbers, not doubles
2. conj may be "N" (no conj.) or "C" (conj.)
********************************************************************/
void _ialglib_vcopy_complex(ae_int_t n, const double *a, ae_int_t stridea, double *b, ae_int_t strideb, const char *conj)
{
    ae_int_t i;

    //
    // more general case
    //
    if( conj[0]=='N' || conj[0]=='n' )
    {
        for(i=0; i<n; i++,a+=2*stridea,b+=2*strideb)
        {
            b[0] = a[0];
            b[1] = a[1];
        }
    }
    else
    {
        for(i=0; i<n; i++,a+=2*stridea,b+=2*strideb)
        {
            b[0] = a[0];
            b[1] = -a[1];
        }
    }
}


/********************************************************************
This subroutine copies matrix from  non-aligned non-contigous storage
to aligned contigous storage

A:
* MxN
* non-aligned
* non-contigous
* may be transformed during copying (as prescribed by op)

B:
* alglib_r_block*alglib_r_block (only MxN/NxM submatrix is used)
* aligned
* stride is alglib_r_block

Transformation types:
* 0 - no transform
* 1 - transposition
********************************************************************/
void _ialglib_mcopyblock(ae_int_t m, ae_int_t n, const double *a, ae_int_t op, ae_int_t stride, double *b)
{
    ae_int_t i, j, n2;
    const double *psrc;
    double *pdst;
    if( op==0 )
    {
        n2 = n/2;
        for(i=0,psrc=a; i<m; i++,a+=stride,b+=alglib_r_block,psrc=a)
        {
            for(j=0,pdst=b; j<n2; j++,pdst+=2,psrc+=2)
            {
                pdst[0] = psrc[0];
                pdst[1] = psrc[1];
            }
            if( n%2!=0 )
                pdst[0] = psrc[0];
        }
    }
    else
    {
        n2 = n/2;
        for(i=0,psrc=a; i<m; i++,a+=stride,b+=1,psrc=a)
        {
            for(j=0,pdst=b; j<n2; j++,pdst+=alglib_twice_r_block,psrc+=2)
            {
                pdst[0] = psrc[0];
                pdst[alglib_r_block] = psrc[1];
            }
            if( n%2!=0 )
                pdst[0] = psrc[0];
        }
    }
}


/********************************************************************
This subroutine copies matrix from  aligned contigous storage to non-
aligned non-contigous storage

A:
* MxN
* aligned
* contigous
* stride is alglib_r_block
* may be transformed during copying (as prescribed by op)

B:
* alglib_r_block*alglib_r_block (only MxN/NxM submatrix is used)
* non-aligned, non-contigous

Transformation types:
* 0 - no transform
* 1 - transposition
********************************************************************/
void _ialglib_mcopyunblock(ae_int_t m, ae_int_t n, const double *a, ae_int_t op, double *b, ae_int_t stride)
{
    ae_int_t i, j, n2;
    const double *psrc;
    double *pdst;
    if( op==0 )
    {
        n2 = n/2;
        for(i=0,psrc=a; i<m; i++,a+=alglib_r_block,b+=stride,psrc=a)
        {
            for(j=0,pdst=b; j<n2; j++,pdst+=2,psrc+=2)
            {
                pdst[0] = psrc[0];
                pdst[1] = psrc[1];
            }
            if( n%2!=0 )
                pdst[0] = psrc[0];
        }
    }
    else
    {
        n2 = n/2;
        for(i=0,psrc=a; i<m; i++,a++,b+=stride,psrc=a)
        {
            for(j=0,pdst=b; j<n2; j++,pdst+=2,psrc+=alglib_twice_r_block)
            {
                pdst[0] = psrc[0];
                pdst[1] = psrc[alglib_r_block];
            }
            if( n%2!=0 )
                pdst[0] = psrc[0];
        }
    }
}


/********************************************************************
This subroutine copies matrix from  non-aligned non-contigous storage
to aligned contigous storage

A:
* MxN
* non-aligned
* non-contigous
* may be transformed during copying (as prescribed by op)
* pointer to ae_complex is passed

B:
* 2*alglib_c_block*alglib_c_block doubles (only MxN/NxM submatrix is used)
* aligned
* stride is alglib_c_block
* pointer to double is passed

Transformation types:
* 0 - no transform
* 1 - transposition
* 2 - conjugate transposition
* 3 - conjugate, but no  transposition
********************************************************************/
void _ialglib_mcopyblock_complex(ae_int_t m, ae_int_t n, const ae_complex *a, ae_int_t op, ae_int_t stride, double *b)
{
    ae_int_t i, j;
    const ae_complex *psrc;
    double *pdst;
    if( op==0 )
    {
        for(i=0,psrc=a; i<m; i++,a+=stride,b+=alglib_twice_c_block,psrc=a)
            for(j=0,pdst=b; j<n; j++,pdst+=2,psrc++)
            {
                pdst[0] = psrc->x;
                pdst[1] = psrc->y;
            }
    }
    if( op==1 )
    {
        for(i=0,psrc=a; i<m; i++,a+=stride,b+=2,psrc=a)
            for(j=0,pdst=b; j<n; j++,pdst+=alglib_twice_c_block,psrc++)
            {
                pdst[0] = psrc->x;
                pdst[1] = psrc->y;
            }
    }
    if( op==2 )
    {
        for(i=0,psrc=a; i<m; i++,a+=stride,b+=2,psrc=a)
            for(j=0,pdst=b; j<n; j++,pdst+=alglib_twice_c_block,psrc++)
            {
                pdst[0] = psrc->x;
                pdst[1] = -psrc->y;
            }
    }
    if( op==3 )
    {
        for(i=0,psrc=a; i<m; i++,a+=stride,b+=alglib_twice_c_block,psrc=a)
            for(j=0,pdst=b; j<n; j++,pdst+=2,psrc++)
            {
                pdst[0] = psrc->x;
                pdst[1] = -psrc->y;
            }
    }
}


/********************************************************************
This subroutine copies matrix from aligned contigous storage to
non-aligned non-contigous storage

A:
* 2*alglib_c_block*alglib_c_block doubles (only MxN submatrix is used)
* aligned
* stride is alglib_c_block
* pointer to double is passed
* may be transformed during copying (as prescribed by op)

B:
* MxN
* non-aligned
* non-contigous
* pointer to ae_complex is passed

Transformation types:
* 0 - no transform
* 1 - transposition
* 2 - conjugate transposition
* 3 - conjugate, but no  transposition
********************************************************************/
void _ialglib_mcopyunblock_complex(ae_int_t m, ae_int_t n, const double *a, ae_int_t op, ae_complex* b, ae_int_t stride)
{
    ae_int_t i, j;
    const double *psrc;
    ae_complex *pdst;
    if( op==0 )
    {
        for(i=0,psrc=a; i<m; i++,a+=alglib_twice_c_block,b+=stride,psrc=a)
            for(j=0,pdst=b; j<n; j++,pdst++,psrc+=2)
            {
                pdst->x = psrc[0];
                pdst->y = psrc[1];
            }
    }
    if( op==1 )
    {
        for(i=0,psrc=a; i<m; i++,a+=2,b+=stride,psrc=a)
            for(j=0,pdst=b; j<n; j++,pdst++,psrc+=alglib_twice_c_block)
            {
                pdst->x = psrc[0];
                pdst->y = psrc[1];
            }
    }
    if( op==2 )
    {
        for(i=0,psrc=a; i<m; i++,a+=2,b+=stride,psrc=a)
            for(j=0,pdst=b; j<n; j++,pdst++,psrc+=alglib_twice_c_block)
            {
                pdst->x = psrc[0];
                pdst->y = -psrc[1];
            }
    }
    if( op==3 )
    {
        for(i=0,psrc=a; i<m; i++,a+=alglib_twice_c_block,b+=stride,psrc=a)
            for(j=0,pdst=b; j<n; j++,pdst++,psrc+=2)
            {
                pdst->x = psrc[0];
                pdst->y = -psrc[1];
            }
    }
}


/********************************************************************
This is real GEMM kernel
********************************************************************/
bool _ialglib_i_rmatrixgemmf(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     double alpha,
     ae_matrix *_a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     ae_matrix *_b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     double beta,
     ae_matrix *_c,
     ae_int_t ic,
     ae_int_t jc)
{
    if( m>alglib_r_block || n>alglib_r_block || k>alglib_r_block )
        return ae_false;

    ae_int_t i, stride, cstride;
    double *crow;
    double __abuf[alglib_r_block+alglib_simd_alignment];
    double __b[alglib_r_block*alglib_r_block+alglib_simd_alignment];
    double * const abuf = (double * const) ae_align(__abuf,alglib_simd_alignment);
    double * const b    = (double * const) ae_align(__b,   alglib_simd_alignment);

    //
    // copy b
    //
    if( optypeb==0 )
        _ialglib_mcopyblock(k, n, &_b->ptr.pp_double[ib][jb], 1, _b->stride, b);
    else
        _ialglib_mcopyblock(n, k, &_b->ptr.pp_double[ib][jb], 0, _b->stride, b);

    //
    // multiply B by A (from the right, by rows)
    // and store result in C
    //
    crow  = &_c->ptr.pp_double[ic][jc];
    stride = _a->stride;
    cstride = _c->stride;
    if( optypea==0 )
    {
        const double *arow = &_a->ptr.pp_double[ia][ja];
        for(i=0; i<m; i++)
        {
            _ialglib_vcopy(k, arow, 1, abuf, 1);
            if( beta==0 )
                _ialglib_vzero(n, crow, 1);
            _ialglib_mv(n, k, b, abuf, crow, 1, alpha, beta);
            crow += cstride;
            arow += stride;
        }
    }
    else
    {
        const double *acol = &_a->ptr.pp_double[ia][ja];
        for(i=0; i<m; i++)
        {
            _ialglib_vcopy(k, acol, stride, abuf, 1);
            if( beta==0 )
                _ialglib_vzero(n, crow, 1);
            _ialglib_mv(n, k, b, abuf, crow, 1, alpha, beta);
            crow += cstride;
            acol++;
        }
    }
    return true;
}


/********************************************************************
complex GEMM kernel
********************************************************************/
bool _ialglib_i_cmatrixgemmf(ae_int_t m,
     ae_int_t n,
     ae_int_t k,
     ae_complex alpha,
     ae_matrix *_a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     ae_matrix *_b,
     ae_int_t ib,
     ae_int_t jb,
     ae_int_t optypeb,
     ae_complex beta,
     ae_matrix *_c,
     ae_int_t ic,
     ae_int_t jc)
 {
    if( m>alglib_c_block || n>alglib_c_block || k>alglib_c_block )
        return false;

    const ae_complex *arow;
    ae_complex *crow;
    ae_int_t i, stride, cstride;
    double __abuf[2*alglib_c_block+alglib_simd_alignment];
    double __b[2*alglib_c_block*alglib_c_block+alglib_simd_alignment];
    double * const abuf = (double * const) ae_align(__abuf,alglib_simd_alignment);
    double * const b    = (double * const) ae_align(__b,   alglib_simd_alignment);

    //
    // copy b
    //
    ae_int_t brows = optypeb==0 ? k : n;
    ae_int_t bcols = optypeb==0 ? n : k;
    if( optypeb==0 )
        _ialglib_mcopyblock_complex(brows, bcols, &_b->ptr.pp_complex[ib][jb], 1, _b->stride, b);
    if( optypeb==1 )
        _ialglib_mcopyblock_complex(brows, bcols, &_b->ptr.pp_complex[ib][jb], 0, _b->stride, b);
    if( optypeb==2 )
        _ialglib_mcopyblock_complex(brows, bcols, &_b->ptr.pp_complex[ib][jb], 3, _b->stride, b);

    //
    // multiply B by A (from the right, by rows)
    // and store result in C
    //
    arow  = &_a->ptr.pp_complex[ia][ja];
    crow  = &_c->ptr.pp_complex[ic][jc];
    stride = _a->stride;
    cstride = _c->stride;
    for(i=0; i<m; i++)
    {
        if( optypea==0 )
        {
            _ialglib_vcopy_complex(k, arow, 1, abuf, 1, "No conj");
            arow += stride;
        }
        else if( optypea==1 )
        {
            _ialglib_vcopy_complex(k, arow, stride, abuf, 1, "No conj");
            arow++;
        }
        else
        {
            _ialglib_vcopy_complex(k, arow, stride, abuf, 1, "Conj");
            arow++;
        }
        if( beta.x==0 && beta.y==0 )
            _ialglib_vzero_complex(n, crow, 1);
        _ialglib_mv_complex(n, k, b, abuf, crow, NULL, 1, alpha, beta);
        crow += cstride;
    }
    return true;
}

/********************************************************************
complex TRSM kernel
********************************************************************/
bool _ialglib_i_cmatrixrighttrsmf(ae_int_t m,
     ae_int_t n,
     ae_matrix *a,
     ae_int_t i1,
     ae_int_t j1,
     bool isupper,
     bool isunit,
     ae_int_t optype,
     ae_matrix *x,
     ae_int_t i2,
     ae_int_t j2)
{
    if( m>alglib_c_block || n>alglib_c_block )
        return false;


    //
    // local buffers
    //
    double *pdiag;
    ae_int_t i;
    double __abuf[2*alglib_c_block*alglib_c_block+alglib_simd_alignment];
    double __xbuf[2*alglib_c_block*alglib_c_block+alglib_simd_alignment];
    double __tmpbuf[2*alglib_c_block+alglib_simd_alignment];
    double * const abuf   = (double * const) ae_align(__abuf,  alglib_simd_alignment);
    double * const xbuf   = (double * const) ae_align(__xbuf,  alglib_simd_alignment);
    double * const tmpbuf = (double * const) ae_align(__tmpbuf,alglib_simd_alignment);

    //
    // Prepare
    //
    bool uppera;
    _ialglib_mcopyblock_complex(n, n, &a->ptr.pp_complex[i1][j1], optype, a->stride, abuf);
    _ialglib_mcopyblock_complex(m, n, &x->ptr.pp_complex[i2][j2], 0, x->stride, xbuf);
    if( isunit )
        for(i=0,pdiag=abuf; i<n; i++,pdiag+=2*(alglib_c_block+1))
        {
            pdiag[0] = 1.0;
            pdiag[1] = 0.0;
        }
    if( optype==0 )
        uppera = isupper;
    else
        uppera = !isupper;

    //
    // Solve Y*A^-1=X where A is upper or lower triangular
    //
    if( uppera )
    {
        for(i=0,pdiag=abuf; i<n; i++,pdiag+=2*(alglib_c_block+1))
        {
            ae_complex tmp_c;
            ae_complex beta;
            ae_complex alpha;
            tmp_c.x = pdiag[0];
            tmp_c.y = pdiag[1];
            beta = ae_c_d_div(1.0, tmp_c);
            alpha.x = -beta.x;
            alpha.y = -beta.y;
            _ialglib_vcopy_complex(i, abuf+2*i, alglib_c_block, tmpbuf, 1, "No conj");
            _ialglib_mv_complex(m, i, xbuf, tmpbuf, NULL, xbuf+2*i, alglib_c_block, alpha, beta);
        }
        _ialglib_mcopyunblock_complex(m, n, xbuf, 0, &x->ptr.pp_complex[i2][j2], x->stride);
    }
    else
    {
        for(i=n-1,pdiag=abuf+2*((n-1)*alglib_c_block+(n-1)); i>=0; i--,pdiag-=2*(alglib_c_block+1))
        {
            ae_complex tmp_c;
            ae_complex beta;
            ae_complex alpha;
            tmp_c.x = pdiag[0];
            tmp_c.y = pdiag[1];
            beta = ae_c_d_div(1.0, tmp_c);
            alpha.x = -beta.x;
            alpha.y = -beta.y;
            _ialglib_vcopy_complex(n-1-i, pdiag+2*alglib_c_block, alglib_c_block, tmpbuf, 1, "No conj");
            _ialglib_mv_complex(m, n-1-i, xbuf+2*(i+1), tmpbuf, NULL, xbuf+2*i, alglib_c_block, alpha, beta);
        }
        _ialglib_mcopyunblock_complex(m, n, xbuf, 0, &x->ptr.pp_complex[i2][j2], x->stride);
    }
    return true;
}

/********************************************************************
real TRSM kernel
********************************************************************/
bool _ialglib_i_rmatrixrighttrsmf(ae_int_t m,
     ae_int_t n,
     ae_matrix *a,
     ae_int_t i1,
     ae_int_t j1,
     bool isupper,
     bool isunit,
     ae_int_t optype,
     ae_matrix *x,
     ae_int_t i2,
     ae_int_t j2)
{
    if( m>alglib_r_block || n>alglib_r_block )
        return false;

    //
    // local buffers
    //
    double *pdiag;
    ae_int_t i;
    double __abuf[alglib_r_block*alglib_r_block+alglib_simd_alignment];
    double __xbuf[alglib_r_block*alglib_r_block+alglib_simd_alignment];
    double __tmpbuf[alglib_r_block+alglib_simd_alignment];
    double * const abuf   = (double * const) ae_align(__abuf,  alglib_simd_alignment);
    double * const xbuf   = (double * const) ae_align(__xbuf,  alglib_simd_alignment);
    double * const tmpbuf = (double * const) ae_align(__tmpbuf,alglib_simd_alignment);

    //
    // Prepare
    //
    bool uppera;
    _ialglib_mcopyblock(n, n, &a->ptr.pp_double[i1][j1], optype, a->stride, abuf);
    _ialglib_mcopyblock(m, n, &x->ptr.pp_double[i2][j2], 0, x->stride, xbuf);
    if( isunit )
        for(i=0,pdiag=abuf; i<n; i++,pdiag+=alglib_r_block+1)
            *pdiag = 1.0;
    if( optype==0 )
        uppera = isupper;
    else
        uppera = !isupper;

    //
    // Solve Y*A^-1=X where A is upper or lower triangular
    //
    if( uppera )
    {
        for(i=0,pdiag=abuf; i<n; i++,pdiag+=alglib_r_block+1)
        {
            double beta  = 1.0/(*pdiag);
            double alpha = -beta;
            _ialglib_vcopy(i, abuf+i, alglib_r_block, tmpbuf, 1);
            _ialglib_mv(m, i, xbuf, tmpbuf, xbuf+i, alglib_r_block, alpha, beta);
        }
        _ialglib_mcopyunblock(m, n, xbuf, 0, &x->ptr.pp_double[i2][j2], x->stride);
    }
    else
    {
        for(i=n-1,pdiag=abuf+(n-1)*alglib_r_block+(n-1); i>=0; i--,pdiag-=alglib_r_block+1)
        {
            double beta = 1.0/(*pdiag);
            double alpha = -beta;
            _ialglib_vcopy(n-1-i, pdiag+alglib_r_block, alglib_r_block, tmpbuf, 1);
            _ialglib_mv(m, n-1-i, xbuf+i+1, tmpbuf, xbuf+i, alglib_r_block, alpha, beta);
        }
        _ialglib_mcopyunblock(m, n, xbuf, 0, &x->ptr.pp_double[i2][j2], x->stride);
    }
    return true;
}

/********************************************************************
complex TRSM kernel
********************************************************************/
bool _ialglib_i_cmatrixlefttrsmf(ae_int_t m,
     ae_int_t n,
     ae_matrix *a,
     ae_int_t i1,
     ae_int_t j1,
     bool isupper,
     bool isunit,
     ae_int_t optype,
     ae_matrix *x,
     ae_int_t i2,
     ae_int_t j2)
{
    if( m>alglib_c_block || n>alglib_c_block )
        return false;
    
    //
    // local buffers
    //
    double *pdiag, *arow;
    ae_int_t i;
    double __abuf[2*alglib_c_block*alglib_c_block+alglib_simd_alignment];
    double __xbuf[2*alglib_c_block*alglib_c_block+alglib_simd_alignment];
    double __tmpbuf[2*alglib_c_block+alglib_simd_alignment];
    double * const abuf   = (double * const) ae_align(__abuf,  alglib_simd_alignment);
    double * const xbuf   = (double * const) ae_align(__xbuf,  alglib_simd_alignment);
    double * const tmpbuf = (double * const) ae_align(__tmpbuf,alglib_simd_alignment);

    //
    // Prepare
    // Transpose X (so we may use mv, which calculates A*x, but not x*A)
    //
    bool uppera;
    _ialglib_mcopyblock_complex(m, m, &a->ptr.pp_complex[i1][j1], optype, a->stride, abuf);
    _ialglib_mcopyblock_complex(m, n, &x->ptr.pp_complex[i2][j2], 1, x->stride, xbuf);
    if( isunit )
        for(i=0,pdiag=abuf; i<m; i++,pdiag+=2*(alglib_c_block+1))
        {
            pdiag[0] = 1.0;
            pdiag[1] = 0.0;
        }
    if( optype==0 )
        uppera = isupper;
    else
        uppera = !isupper;

    //
    // Solve A^-1*Y^T=X^T where A is upper or lower triangular
    //
    if( uppera )
    {
        for(i=m-1,pdiag=abuf+2*((m-1)*alglib_c_block+(m-1)); i>=0; i--,pdiag-=2*(alglib_c_block+1))
        {
            ae_complex tmp_c;
            ae_complex beta;
            ae_complex alpha;
            tmp_c.x = pdiag[0];
            tmp_c.y = pdiag[1];
            beta = ae_c_d_div(1.0, tmp_c);
            alpha.x = -beta.x;
            alpha.y = -beta.y;
            _ialglib_vcopy_complex(m-1-i, pdiag+2, 1, tmpbuf, 1, "No conj");
            _ialglib_mv_complex(n, m-1-i, xbuf+2*(i+1), tmpbuf, NULL, xbuf+2*i, alglib_c_block, alpha, beta);
        }
        _ialglib_mcopyunblock_complex(m, n, xbuf, 1, &x->ptr.pp_complex[i2][j2], x->stride);
    }
    else
    {   for(i=0,pdiag=abuf,arow=abuf; i<m; i++,pdiag+=2*(alglib_c_block+1),arow+=2*alglib_c_block)
        {
            ae_complex tmp_c;
            ae_complex beta;
            ae_complex alpha;
            tmp_c.x = pdiag[0];
            tmp_c.y = pdiag[1];
            beta = ae_c_d_div(1.0, tmp_c);
            alpha.x = -beta.x;
            alpha.y = -beta.y;
            _ialglib_vcopy_complex(i, arow, 1, tmpbuf, 1, "No conj");
            _ialglib_mv_complex(n, i, xbuf, tmpbuf, NULL, xbuf+2*i, alglib_c_block, alpha, beta);
        }
        _ialglib_mcopyunblock_complex(m, n, xbuf, 1, &x->ptr.pp_complex[i2][j2], x->stride);
    }
    return true;
}


/********************************************************************
real TRSM kernel
********************************************************************/
bool _ialglib_i_rmatrixlefttrsmf(ae_int_t m,
     ae_int_t n,
     ae_matrix *a,
     ae_int_t i1,
     ae_int_t j1,
     bool isupper,
     bool isunit,
     ae_int_t optype,
     ae_matrix *x,
     ae_int_t i2,
     ae_int_t j2)
{
    if( m>alglib_r_block || n>alglib_r_block )
        return false;
    
    //
    // local buffers
    //
    double *pdiag, *arow;
    ae_int_t i;
    double __abuf[alglib_r_block*alglib_r_block+alglib_simd_alignment];
    double __xbuf[alglib_r_block*alglib_r_block+alglib_simd_alignment];
    double __tmpbuf[alglib_r_block+alglib_simd_alignment];
    double * const abuf   = (double * const) ae_align(__abuf,  alglib_simd_alignment);
    double * const xbuf   = (double * const) ae_align(__xbuf,  alglib_simd_alignment);
    double * const tmpbuf = (double * const) ae_align(__tmpbuf,alglib_simd_alignment);

    //
    // Prepare
    // Transpose X (so we may use mv, which calculates A*x, but not x*A)
    //
    bool uppera;
    _ialglib_mcopyblock(m, m, &a->ptr.pp_double[i1][j1], optype, a->stride, abuf);
    _ialglib_mcopyblock(m, n, &x->ptr.pp_double[i2][j2], 1, x->stride, xbuf);
    if( isunit )
        for(i=0,pdiag=abuf; i<m; i++,pdiag+=alglib_r_block+1)
            *pdiag = 1.0;
    if( optype==0 )
        uppera = isupper;
    else
        uppera = !isupper;

    //
    // Solve A^-1*Y^T=X^T where A is upper or lower triangular
    //
    if( uppera )
    {
        for(i=m-1,pdiag=abuf+(m-1)*alglib_r_block+(m-1); i>=0; i--,pdiag-=alglib_r_block+1)
        {
            double beta = 1.0/(*pdiag);
            double alpha = -beta;
            _ialglib_vcopy(m-1-i, pdiag+1, 1, tmpbuf, 1);
            _ialglib_mv(n, m-1-i, xbuf+i+1, tmpbuf, xbuf+i, alglib_r_block, alpha, beta);
        }
        _ialglib_mcopyunblock(m, n, xbuf, 1, &x->ptr.pp_double[i2][j2], x->stride);
    }
    else
    {   for(i=0,pdiag=abuf,arow=abuf; i<m; i++,pdiag+=alglib_r_block+1,arow+=alglib_r_block)
        {
            double beta = 1.0/(*pdiag);
            double alpha = -beta;
            _ialglib_vcopy(i, arow, 1, tmpbuf, 1);
            _ialglib_mv(n, i, xbuf, tmpbuf, xbuf+i, alglib_r_block, alpha, beta);
        }
        _ialglib_mcopyunblock(m, n, xbuf, 1, &x->ptr.pp_double[i2][j2], x->stride);
    }
    return true;
}


/********************************************************************
complex SYRK kernel
********************************************************************/
bool _ialglib_i_cmatrixsyrkf(ae_int_t n,
     ae_int_t k,
     double alpha,
     ae_matrix *a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     ae_matrix *c,
     ae_int_t ic,
     ae_int_t jc,
     bool isupper)
{
    if( n>alglib_c_block || k>alglib_c_block )
        return false;
    if( n==0 )
        return true;
    
    //
    // local buffers
    //
    double *arow, *crow;
    ae_complex c_alpha, c_beta;
    ae_int_t i;
    double __abuf[2*alglib_c_block*alglib_c_block+alglib_simd_alignment];
    double __cbuf[2*alglib_c_block*alglib_c_block+alglib_simd_alignment];
    double __tmpbuf[2*alglib_c_block+alglib_simd_alignment];
    double * const abuf   = (double * const) ae_align(__abuf,  alglib_simd_alignment);
    double * const cbuf   = (double * const) ae_align(__cbuf,  alglib_simd_alignment);
    double * const tmpbuf = (double * const) ae_align(__tmpbuf,alglib_simd_alignment);

    //
    // copy A and C, task is transformed to "A*A^H"-form.
    // if beta==0, then C is filled by zeros (and not referenced)
    //
    // alpha==0 or k==0 are correctly processed (A is not referenced)
    //
    c_alpha.x = alpha;
    c_alpha.y = 0;
    c_beta.x = beta;
    c_beta.y = 0;
    if( alpha==0 )
        k = 0;
    if( k>0 )
    {
        if( optypea==0 )
            _ialglib_mcopyblock_complex(n, k, &a->ptr.pp_complex[ia][ja], 3, a->stride, abuf);
        else
            _ialglib_mcopyblock_complex(k, n, &a->ptr.pp_complex[ia][ja], 1, a->stride, abuf);
    }
    _ialglib_mcopyblock_complex(n, n, &c->ptr.pp_complex[ic][jc], 0, c->stride, cbuf);
    if( beta==0 )
    {
        for(i=0,crow=cbuf; i<n; i++,crow+=2*alglib_c_block)
            if( isupper )
                _ialglib_vzero(2*(n-i), crow+2*i, 1);
            else
                _ialglib_vzero(2*(i+1), crow, 1);
    }


    //
    // update C
    //
    if( isupper )
    {
        for(i=0,arow=abuf,crow=cbuf; i<n; i++,arow+=2*alglib_c_block,crow+=2*alglib_c_block)
        {
            _ialglib_vcopy_complex(k, arow, 1, tmpbuf, 1, "Conj");
            _ialglib_mv_complex(n-i, k, arow, tmpbuf, NULL, crow+2*i, 1, c_alpha, c_beta);
        }
    }
    else
    {
        for(i=0,arow=abuf,crow=cbuf; i<n; i++,arow+=2*alglib_c_block,crow+=2*alglib_c_block)
        {
            _ialglib_vcopy_complex(k, arow, 1, tmpbuf, 1, "Conj");
            _ialglib_mv_complex(i+1, k, abuf, tmpbuf, NULL, crow, 1, c_alpha, c_beta);
        }
    }

    //
    // copy back
    //
    _ialglib_mcopyunblock_complex(n, n, cbuf, 0, &c->ptr.pp_complex[ic][jc], c->stride);

    return true;
}


/********************************************************************
real SYRK kernel
********************************************************************/
bool _ialglib_i_rmatrixsyrkf(ae_int_t n,
     ae_int_t k,
     double alpha,
     ae_matrix *a,
     ae_int_t ia,
     ae_int_t ja,
     ae_int_t optypea,
     double beta,
     ae_matrix *c,
     ae_int_t ic,
     ae_int_t jc,
     bool isupper)
{
    if( n>alglib_r_block || k>alglib_r_block )
        return false;
    if( n==0 )
        return true;
    
    //
    // local buffers
    //
    double *arow, *crow;
    ae_int_t i;
    double __abuf[alglib_r_block*alglib_r_block+alglib_simd_alignment];
    double __cbuf[alglib_r_block*alglib_r_block+alglib_simd_alignment];
    double * const abuf   = (double * const) ae_align(__abuf,  alglib_simd_alignment);
    double * const cbuf   = (double * const) ae_align(__cbuf,  alglib_simd_alignment);

    //
    // copy A and C, task is transformed to "A*A^T"-form.
    // if beta==0, then C is filled by zeros (and not referenced)
    //
    // alpha==0 or k==0 are correctly processed (A is not referenced)
    //
    if( alpha==0 )
        k = 0;
    if( k>0 )
    {
        if( optypea==0 )
            _ialglib_mcopyblock(n, k, &a->ptr.pp_double[ia][ja], 0, a->stride, abuf);
        else
            _ialglib_mcopyblock(k, n, &a->ptr.pp_double[ia][ja], 1, a->stride, abuf);
    }
    _ialglib_mcopyblock(n, n, &c->ptr.pp_double[ic][jc], 0, c->stride, cbuf);
    if( beta==0 )
    {
        for(i=0,crow=cbuf; i<n; i++,crow+=alglib_r_block)
            if( isupper )
                _ialglib_vzero(n-i, crow+i, 1);
            else
                _ialglib_vzero(i+1, crow, 1);
    }


    //
    // update C
    //
    if( isupper )
    {
        for(i=0,arow=abuf,crow=cbuf; i<n; i++,arow+=alglib_r_block,crow+=alglib_r_block)
        {
            _ialglib_mv(n-i, k, arow, arow, crow+i, 1, alpha, beta);
        }
    }
    else
    {
        for(i=0,arow=abuf,crow=cbuf; i<n; i++,arow+=alglib_r_block,crow+=alglib_r_block)
        {
            _ialglib_mv(i+1, k, abuf, arow, crow, 1, alpha, beta);
        }
    }

    //
    // copy back
    //
    _ialglib_mcopyunblock(n, n, cbuf, 0, &c->ptr.pp_double[ic][jc], c->stride);

    return true;
}


/********************************************************************
complex rank-1 kernel
********************************************************************/
bool _ialglib_i_cmatrixrank1f(ae_int_t m,
     ae_int_t n,
     ae_matrix *a,
     ae_int_t ia,
     ae_int_t ja,
     ae_vector *u,
     ae_int_t uoffs,
     ae_vector *v,
     ae_int_t voffs)
{
    ae_complex *arow, *pu, *pv, *vtmp, *dst;
    ae_int_t n2 = n/2;
    ae_int_t stride  = a->stride;
    ae_int_t i, j;

    //
    // update pairs of rows
    //
    arow  = &a->ptr.pp_complex[ia][ja];
    pu    = &u->ptr.p_complex[uoffs];
    vtmp  = &v->ptr.p_complex[voffs];
    for(i=0; i<m; i++, arow+=stride, pu++)
    {
        //
        // update by two
        //
        for(j=0,pv=vtmp, dst=arow; j<n2; j++, dst+=2, pv+=2)
        {
            double ux  = pu[0].x;
            double uy  = pu[0].y;
            double v0x = pv[0].x;
            double v0y = pv[0].y;
            double v1x = pv[1].x;
            double v1y = pv[1].y;
            dst[0].x += ux*v0x-uy*v0y;
            dst[0].y += ux*v0y+uy*v0x;
            dst[1].x += ux*v1x-uy*v1y;
            dst[1].y += ux*v1y+uy*v1x;
            //dst[0] += pu[0]*pv[0];
            //dst[1] += pu[0]*pv[1];
        }

        //
        // final update
        //
        if( n%2!=0 )
        {
            double ux = pu[0].x;
            double uy = pu[0].y;
            double vx = pv[0].x;
            double vy = pv[0].y;
            dst[0].x += ux*vx-uy*vy;
            dst[0].y += ux*vy+uy*vx;
        }
    }
    return true;
}


/********************************************************************
real rank-1 kernel
********************************************************************/
bool _ialglib_i_rmatrixrank1f(ae_int_t m,
     ae_int_t n,
     ae_matrix *a,
     ae_int_t ia,
     ae_int_t ja,
     ae_vector *u,
     ae_int_t uoffs,
     ae_vector *v,
     ae_int_t voffs)
{
    double *arow0, *arow1, *pu, *pv, *vtmp, *dst0, *dst1;
    ae_int_t m2 = m/2;
    ae_int_t n2 = n/2;
    ae_int_t stride  = a->stride;
    ae_int_t stride2 = 2*a->stride;
    ae_int_t i, j;

    //
    // update pairs of rows
    //
    arow0 = &a->ptr.pp_double[ia][ja];
    arow1 = arow0+stride;
    pu    = &u->ptr.p_double[uoffs];
    vtmp  = &v->ptr.p_double[voffs];
    for(i=0; i<m2; i++,arow0+=stride2,arow1+=stride2,pu+=2)
    {
        //
        // update by two
        //
        for(j=0,pv=vtmp, dst0=arow0, dst1=arow1; j<n2; j++, dst0+=2, dst1+=2, pv+=2)
        {
            dst0[0] += pu[0]*pv[0];
            dst0[1] += pu[0]*pv[1];
            dst1[0] += pu[1]*pv[0];
            dst1[1] += pu[1]*pv[1];
        }

        //
        // final update
        //
        if( n%2!=0 )
        {
            dst0[0] += pu[0]*pv[0];
            dst1[0] += pu[1]*pv[0];
        }
    }

    //
    // update last row
    //
    if( m%2!=0 )
    {
        //
        // update by two
        //
        for(j=0,pv=vtmp, dst0=arow0; j<n2; j++, dst0+=2, pv+=2)
        {
            dst0[0] += pu[0]*pv[0];
            dst0[1] += pu[0]*pv[1];
        }

        //
        // final update
        //
        if( n%2!=0 )
            dst0[0] += pu[0]*pv[0];
    }
    return true;
}

}
