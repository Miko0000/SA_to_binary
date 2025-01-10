#ifndef __RESOURCE_gr_kw_H__
#define __RESOURCE_gr_kw_H__

#include <gio/gio.h>

extern GResource *gr_kw_get_resource (void);

extern void gr_kw_register_resource (void);
extern void gr_kw_unregister_resource (void);

#endif
