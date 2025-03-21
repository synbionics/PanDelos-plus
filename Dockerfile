FROM ubuntu:latest

RUN apt update && apt-get upgrade -y && \
    apt install -y \
        python3 \
        python3-pip \
        nano \
        time \
        curl \
        htop \
        gzip\ 
        bash \
        git \
        build-essential \
        unzip

RUN useradd -ms /bin/bash pdp

ARG TOOLNAME="PanDelos-plus"
ARG VERSION="1.0.0"

ARG PDPGITHUB="https://github.com/synbionics/${TOOLNAME}/archive/refs/tags/v${VERSION}.zip"
ARG WORKDIR="/home/pdp"

ARG TOOLDIR="/home/pdp/${TOOLNAME}"

ARG ZIPNAME="pdp.zip"

RUN mkdir -p ${WORKDIR}
WORKDIR ${WORKDIR}


RUN curl -L --output ${ZIPNAME} ${PDPGITHUB}
RUN unzip ${ZIPNAME} 

RUN mv "${TOOLNAME}-${VERSION}" ${TOOLNAME}

WORKDIR ${WORKDIR}/${TOOLNAME}


RUN bash compile.sh
COPY pip-requirements.txt pip-requirements.txt
RUN pip install --break-system-packages -r pip-requirements.txt

ARG OUTDIR="${TOOLDIR}/input"
RUN mkdir -p ${OUTDIR}
WORKDIR ${OUTDIR}

ARG OUTDIR="${TOOLDIR}/output"
RUN mkdir -p ${OUTDIR}
WORKDIR ${OUTDIR}

RUN chown -R pdp:pdp ${TOOLDIR}
RUN chmod -R 755 ${TOOLDIR}

RUN echo 'PS1="\[\033[01;34m\]\u@\h:\[\033[01;32m\]\w\[\033[00m\]\$ "' >> /home/pdp/.bashrc && \
    echo 'alias ll="ls -lh --color=auto"' >> /home/pdp/.bashrc && \
    echo 'alias la="ls -lha --color=auto"' >> /home/pdp/.bashrc && \
    echo 'export LS_COLORS="di=01;34:ln=01;36:so=01;35:pi=33:ex=01;32:bd=40;33;01:cd=40;33;01"' >> /home/pdp/.bashrc


WORKDIR ${TOOLDIR}

USER pdp
