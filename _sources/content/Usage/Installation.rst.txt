.. include:: ../substitutions.rst

.. _installation:

============
Installation
============

The |os-matcher| uses Conan for installing third-party libraries.
Some Conan packages aren't in any public registry yet and have to be made available in your company's infrastructure or through a local registry. We will use the latter approach in this guide.

Requirements
============

.. warning::

   For the following installation guide we assume you have a running
   Debian based system (f.ex. Ubuntu) and the following tools
   installed and configured:

   - Git
   - GCC 5+
   - Python 3

We need additional build tools and the PostgreSQL library.

.. code-block:: bash

   sudo apt install ninja-build cmake iwyu libpq-dev
   python3 -m pip install conan --upgrade

Furthermore we need to set up a Conan profile used to get packages.
We re-create the default profile now, configured for GCC and the new C++11 ABI.

.. code-block:: bash

   # Create default profile configured for GCC and new C++11 ABI
   rm -f ~/.conan/profiles/default
   conan profile new default --detect
   conan profile update settings.compiler.libcxx=libstdc++11 default

Getting the code
================

Change into some workspace directory and then clone the repository.

.. code:: bash

   git clone https://github.com/Ambrosys/os-matcher.git

Provision Conan packages
========================

We run a local Conan registry in a seperate terminal.

.. code:: bash

   cd conan
   ./run-server.sh

Configure Conan to use this registry when refering to ``ambrosys``.

.. code:: bash

   remote_url=http://localhost:9300/
   username=demo
   password=demo

   conan remote add ambrosys $remote_url
   conan user --remote ambrosys $username -p $password

Now we can download, build and publish packages into our local registry.

.. code:: bash

   mkdir conan/repositories
   for package in amb-graph amb-log amb-thread amb-pipeline cli-app lemon; do
       git clone https://github.com/Ambrosys/$package.git conan/repositories/$package
       conan remove --force "$package/*" --src --builds --packages
       conan remove --force "$package/*" --system-reqs
       conan remove --force "$package/*" --remote ambrosys
       conan create conan/repositories/$package amb/stable --build $package --build missing
       conan upload "$package/*" --confirm --all --remote ambrosys
   done
  
In case that does not work, use  git@github.com:Ambrosys/$package.git instead of https://github.com/Ambrosys/$package.git

Building |os-matcher|
=====================

Now that all dependencies are at hand, we can finally build the |os-matcher| code.

.. code:: bash

   mkdir build
   cmake -Bbuild -H. -GNinja \
       -DCMAKE_BUILD_TYPE=Release \
       -DCMAKE_INSTALL_PREFIX=install
   ninja -C build test-if-git-HEAD-has-changed
   ninja -C build install

You will find the binaries now in ``build/install``.
git@github.com:Ambrosys/$package.git
