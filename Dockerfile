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
        build-essential

RUN useradd -ms /bin/bash pdp

ARG WORKDIR="/home/pdp/workdir"
RUN mkdir -p ${WORKDIR}
WORKDIR ${WORKDIR}

COPY . .
RUN bash compile.sh
COPY pip-requirements.txt pip-requirements.txt
RUN pip install --break-system-packages -r pip-requirements.txt

ARG OUTDIR="/home/pdp/workdir/input"
RUN mkdir -p ${OUTDIR}
WORKDIR ${OUTDIR}

ARG OUTDIR="/home/pdp/workdir/output"
RUN mkdir -p ${OUTDIR}
WORKDIR ${OUTDIR}

RUN chown -R pdp:pdp ${WORKDIR}
RUN chmod -R 755 ${WORKDIR}

RUN echo 'PS1="\[\033[01;34m\]\u@\h:\[\033[01;32m\]\w\[\033[00m\]\$ "' >> /home/pdp/.bashrc && \
    echo 'alias ll="ls -lh --color=auto"' >> /home/pdp/.bashrc && \
    echo 'alias la="ls -lha --color=auto"' >> /home/pdp/.bashrc && \
    echo 'export LS_COLORS="di=01;34:ln=01;36:so=01;35:pi=33:ex=01;32:bd=40;33;01:cd=40;33;01"' >> /home/pdp/.bashrc


WORKDIR ${WORKDIR}

USER pdp
