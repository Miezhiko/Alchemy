#!/bin/bash

# Register a protocol handler (default: handle_secondlifeprotocol.sh) for
# URLs of the form secondlife://...
#

HANDLER="$1"

RUN_PATH=`dirname "$0" || echo .`
cd "${RUN_PATH}/.."

if [ -z "$HANDLER" ]; then
    HANDLER=`pwd`/etc/handle_secondlifeprotocol.sh
fi

# Register handler for GNOME-aware apps
LLXDGTOOL=xdg-mime
if [ `which $LLXDGTOOL` ]; then
    XDGDIR="$(xdg-user-dir)/.local/share/applications"
    mkdir -p "${XDGDIR}"
    LLXDGPROTOFILE=${XDGDIR}/secondlife.desktop
    cat > ${LLXDGPROTOFILE} <<EOF || echo Warning: Did not register secondlife:// handler with XDG: Could not write ${LLXDGPROTOFILE} 
[Desktop Entry]
Name=Second Life
Exec=${HANDLER} '%u'
Type=Application
Terminal=false
MimeType=x-scheme-handler/secondlife;
EOF
$LLXDGTOOL default secondlife.desktop x-scheme-handler/secondlife
fi

# Register handler for KDE-aware apps
for LLKDECONFIG in kde-config kde4-config; do
    if [ `which $LLKDECONFIG` ]; then
        LLKDEPROTODIR=`$LLKDECONFIG --path services | cut -d ':' -f 1`
        if [ -d "$LLKDEPROTODIR" ]; then
            LLKDEPROTOFILE=${LLKDEPROTODIR}/secondlife.protocol
            cat > ${LLKDEPROTOFILE} <<EOF || echo Warning: Did not register secondlife:// handler with KDE: Could not write ${LLKDEPROTOFILE} 
[Protocol]
exec=${HANDLER} '%u'
protocol=secondlife
input=none
output=none
helper=true
listing=
reading=false
writing=false
makedir=false
deleting=false
EOF
        else
            echo Warning: Did not register secondlife:// handler with KDE: Directory $LLKDEPROTODIR does not exist.
        fi
    fi
done
