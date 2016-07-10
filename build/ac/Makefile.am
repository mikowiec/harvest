
AUTOMAKE_OPTIONS = subdir-objects

rp_data_dir=$(top_srcdir)/../../data

data_dirs = ai config levels music objectdata objects scenario shaders sounds terrains textures

rp_vs_dir = $(top_srcdir)/../vs

EXTRA_DIST = \
    $(rp_vs_dir)/reaper/reaper.dsp \
    $(rp_vs_dir)/reaper/plugin_gfx_win32.dsp \
    $(rp_vs_dir)/reaper/plugin_event_win32.dsp \
    $(rp_vs_dir)/reaper/plugin_snd_wave.dsp \
    $(rp_vs_dir)/reaper/plugin_snd_mp3.dsp \
    $(rp_vs_dir)/reaper/plugin_snd_openal.dsp \
    $(rp_vs_dir)/reaper/plugin_snd_simple.dsp \
    $(rp_vs_dir)/reaper/plugin_snd_dsound.dsp \
    $(rp_vs_dir)/reaper/plugin_snd_dummy.dsp \
    $(rp_vs_dir)/ext/mpegsound_static/mpegsound_static.dsp \
    $(rp_vs_dir)/public/reaper_demo/reaper_demo.dsp \
    $(rp_vs_dir)/work/reaper.dsw

rp_src_dir=$(srcdir)/../../src

SUBDIRS = ext game ai gfx os main misc net object phys res snd world public msg

bin_PROGRAMS = reaper

reaper_SOURCES  = 
reaper_CXXFLAGS = $(cxxflags)
reaper_LDFLAGS  = $(NET_LIBS) $(X11_LIBS)
reaper_LDADD    = libreaper.a

subdir_libs = \
    ai/libai.la ext/libmpegsound.la game/libgame.la \
    gfx/cloud/libgfx_cloud.la gfx/pm/libgfx_pm.la \
    gfx/shadow/libgfx_shadow.la gfx/libgfx.la \
    os/libos.la os/@THREAD_LIB@ \
    os/gfx/libos_gfx.la os/snd/libos_snd.la \
    os/event/libos_event.la msg/libmsg.la \
    os/net/libos_net.la os/gl/libos_gl.la \
    main/libmain.la misc/libmisc.la net/libnet.la \
    object/libobject.la phys/libphys.la \
    res/libres.la snd/libsnd.la world/libworld.la \
    public/prototype/libpublic_prototype.la

libreaper.a: $(subdir_libs)
	-rm -f data
	-ln -s $(srcdir)/../../data data
	-rm -f plugins
	-mkdir plugins
	-cp os/*/.libs/*.so plugins
	-rm -rf junk && mkdir junk && cd junk && \
	libs="$(subdir_libs)"; \
            for lib in $$libs ; do \
                dir=`echo $$lib | sed 's!/[^/]*$$!!'` ; \
                real_lib=`cat ../$$lib | grep old_library | cut -f2 -d\' ` ; \
                $(AR) x ../$$dir/.libs/$$real_lib ; \
            done && \
            $(AR) rcs ../libreaper.a * && \
            $(RANLIB) ../libreaper.a && \
            cd .. && rm -rf junk




# Install all data files in the directories mentioned in $(data_dirs)
# while avoiding CVS files.

install-data-local:
	echo "Installing datafiles..."
	@for d in $(data_dirs) ; do \
            $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/$$d ; \
            files=`find $(rp_data_dir)/$$d -type f -not -name CVS` ; \
            for f in $$files ; do \
                fn=`basename $$f` ; \
                $(INSTALL_DATA) $$f $(DESTDIR)$(pkgdatadir)/$$d/$$fn ; \
            done ; \
        done





