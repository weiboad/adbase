Summary: @ADINF_PROJECT_SUMMARY@
Name: @ADINF_PROJECT_NAME@
Version:${ADINFVERSION}
Release:${OPVERSION}
Group:Development/Tools
License:BSD
URL: @ADINF_PROJECT_URL@
Vendor: @ADINF_PROJECT_VENDOR@
Packager: @ADINF_PROJECT_VENDOR@
Distribution: @ADINF_PROJECT_SUMMARY@
Source:%{name}-%{version}.tar
Buildroot:%{_tmppath}/%{name}-%{version}
Prefix:/usr/local/adinf/@ADINF_PROJECT_NAME@
Requires:sudo
%define debug_packages %{nil}
%define debug_package %{nil}
%define _topdir /usr/home/zhongxiu/rpmbuild
%description
-------------------------------------
- Everything in order to facilitate -
-------------------------------------

%prep
%setup -q
%build

%pre
if test -d %{prefix}; then
	echo ""
	echo "Install dir \'%{prefix}\' exists. You need resolve it manually before install @ADINF_PROJECT_NAME@. Exit now."
	echo ""
# return !0 for exit.
	test ""
fi
%install
#rm -rf $RPM_BUILD_ROOT
mkdir -p $RPM_BUILD_ROOT%{prefix}
cp -r * $RPM_BUILD_ROOT%{prefix}

%clean
#rm -rf $RPM_BUILD_ROOT

%post
# build install
echo ""
echo "Build: START"
echo -e "Build: [0;32mDONE[0m"
%preun
echo -n "unrpm  ... "
SW_TMP_BAK_PATH=%{prefix}-backup-$(date +%%Y%%m%%d-%%H%%M%%S)
mv %{prefix} $SW_TMP_BAK_PATH
echo "Old version '%{prefix}' backup to '$SW_TMP_BAK_PATH'"
%postun
%files
%{prefix}

%changelog

