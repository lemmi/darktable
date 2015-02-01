/*
   This file is part of darktable,
   copyright (c) 2015 Jeremy Rosen

   darktable is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   darktable is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with darktable.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "lua/widget/common.h"
#include "lua/types.h"
#include "gui/gtk.h"

static void container_init(lua_State* L);
dt_lua_widget_type_t container_type = {
  .name = "container",
  .gui_init = container_init,
  .gui_cleanup = NULL,
  .alloc_size = sizeof(dt_lua_container_t),
  .parent= &widget_type
};

static int container_reset(lua_State* L)
{
  lua_container container;
  luaA_to(L,lua_container,&container,1);
  lua_getuservalue(L,1);
  GList*children = gtk_container_get_children(GTK_CONTAINER(container->widget));
  GList*curelt = children;
  while(curelt) {
    GtkWidget* cur_widget = curelt->data;
    lua_pushlightuserdata(L,cur_widget);
    lua_gettable(L,-2);
    lua_widget data;
    luaA_to(L,lua_widget,&data,-1);
    dt_lua_widget_trigger_callback(L,data,"reset");
    lua_pop(L,1);
    curelt = g_list_next(curelt);
  }
  g_list_free(children);
  return 0;
}

static void container_init(lua_State* L)
{
  lua_container container;
  luaA_to(L,lua_container,&container,-1);
  lua_pushcfunction(L,container_reset);
  dt_lua_widget_set_callback(L,-2,"reset");
}


static int container_append(lua_State *L)
{
  lua_container container;
  luaA_to(L,lua_container,&container,1);
  lua_widget widget;
  luaA_to(L, lua_widget,&widget,2),
  gtk_container_add(GTK_CONTAINER(container->widget),widget->widget);
  lua_getuservalue(L,1);
  lua_pushlightuserdata(L,widget->widget);
  lua_pushvalue(L,2);
  lua_settable(L,-3);
  lua_pop(L,1);
  return 0;
}

static int container_len(lua_State*L)
{
  lua_container container;
  luaA_to(L,lua_container,&container,1);
  GList * children = gtk_container_get_children(GTK_CONTAINER(container->widget));
  lua_pushinteger(L,g_list_length(children));
  g_list_free(children);
  return 1;
}

static int container_numindex(lua_State*L)
{
  lua_container container;
  luaA_to(L,lua_container,&container,1);
  GList * children = gtk_container_get_children(GTK_CONTAINER(container->widget));
  GtkWidget *searched_widget = g_list_nth_data(children,lua_tointeger(L,2)-1);
  g_list_free(children);
  lua_getuservalue(L,1);
  lua_pushlightuserdata(L,searched_widget);
  lua_gettable(L,-2);
  return 1;
}

int dt_lua_init_widget_container(lua_State* L)
{
  dt_lua_init_widget_type(L,&container_type,lua_container,GTK_TYPE_CONTAINER);
  lua_pushcfunction(L, container_append);
  lua_pushcclosure(L, dt_lua_type_member_common, 1);
  dt_lua_type_register_const(L, lua_container, "append");
  lua_pushcfunction(L,container_len);
  lua_pushcclosure(L,dt_lua_gtk_wrap,1);
  lua_pushcfunction(L,container_numindex);
  dt_lua_type_register_number_const(L,lua_container);

  return 0;
}
// modelines: These editor modelines have been set for all relevant files by tools/update_modelines.sh
// vim: shiftwidth=2 expandtab tabstop=2 cindent
// kate: tab-indents: off; indent-width 2; replace-tabs on; indent-mode cstyle; remove-trailing-space on;