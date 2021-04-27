TARGETS=lemon
BOOST_TOOLSET=gcc
ifeq ($(findstring clang,${CXX}),clang)
        BOOST_TOOLSET=clang
endif

THIRDPARTY_ROOT := $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

INSTALL_DIR=${THIRDPARTY_ROOT}/install

CXXFLAGS:=${CXXFLAGS} -D_GLIBCXX_USE_CXX11_ABI=0

CLEAN_BUILD ?= false

.PHONY : all clean gitclean allfast allfastbydirhash allfastbydirhash ${TARGETS}

allfast: allfastbydirhash

all: allfast


allfastbygithead:
	@mkdir -p "${INSTALL_DIR}/buildinfo" || true
	@for subtarget in ${TARGETS} ; \
	do \
		cd "${THIRDPARTY_ROOT}/$${subtarget}" ; \
		export HEAD=$$(git rev-parse HEAD) ; \
		export MARKERFILE="${INSTALL_DIR}/buildinfo/$${subtarget}.githead.$${HEAD}" ; \
		test -f "$${MARKERFILE}" && \
			echo "$${subtarget} target skiped build. Build was marked as done and successfull by maker file $${MARKERFILE}" && \
			continue ; \
		echo "No maker file found therefore goinig to build $${subtarget} target" ; \
		@rm "${INSTALL_DIR}/buildinfo/$${subtarget}.githead."* 2> /dev/null || true ; \
		$(MAKE) -C ${THIRDPARTY_ROOT} -j${NJOBS} $${subtarget} && \
			touch "$${MARKERFILE}" || exit 1 ; \
	done

allfastbydirhash:
	@mkdir -p "${INSTALL_DIR}/buildinfo" || true
	@for subtarget in ${TARGETS} ; \
	do \
		cd "${THIRDPARTY_ROOT}/$${subtarget}" ; \
		export DIRHASH=$$(ls -alR . ../Makefile | sha1sum | awk '{print $$1}') ; \
		export MARKERFILE="${INSTALL_DIR}/buildinfo/$${subtarget}.dirhash.$${DIRHASH}"  ; \
		test -f "$${MARKERFILE}" && \
			echo "$${subtarget} target skiped build. Build was marked as done and successfull by maker file $${MARKERFILE}" && \
			continue ; \
		echo "No maker file found therefore goinig to build $${subtarget} target" ; \
		@rm "${INSTALL_DIR}/buildinfo/$${subtarget}.dirhash."* 2> /dev/null || true ; \
		$(MAKE) -C ${THIRDPARTY_ROOT} -j${NJOBS} $${subtarget} && \
		( \
			export DIRHASH=$$(ls -alR . ../Makefile | sha1sum | awk '{print $$1}') ; \
			export MARKERFILE="${INSTALL_DIR}/buildinfo/$${subtarget}.dirhash.$${DIRHASH}"  ; \
			touch "$${MARKERFILE}" ; \
		) || exit 1 ; \
	done


lemon: export CXXFLAGS:=${CXXFLAGS}
lemon:
	@${CLEAN_BUILD} && rm -rf "$@/build" &> /dev/null || true
	@mkdir "$@/build" || true
	cd "$@/build" && cmake .. -DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}" -DLEMON_ENABLE_COIN=NO -DLEMON_ENABLE_ILOG=NO -DLEMON_ENABLE_SOPLEX=NO
	$(MAKE) -j4 -C "$@/build" install


clean:
	$(MAKE) -C lemon/build clean
	rm -rf "${INSTALL_DIR}" ; mkdir "${INSTALL_DIR}"

gitclean:
	for f in ${TARGETS}; do ( cd $$f && git clean -f -d -x || true ) ; done
