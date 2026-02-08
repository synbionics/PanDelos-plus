FROM ubuntu:latest

RUN apt update && apt-get upgrade -y && \
    apt install -y \
        python3 \
        python3-pip \
        nano \
        time \
        curl \
        htop \
        gzip \
        bash \
        git \
        build-essential \
        unzip

RUN useradd -ms /bin/bash pdp

ARG TOOLNAME="PanDelos-plus"
ARG WORKDIR="/home/pdp"
ARG TOOLDIR="/home/pdp/${TOOLNAME}"

RUN mkdir -p ${TOOLDIR}
WORKDIR ${TOOLDIR}

# Install Python dependencies first (cached unless pip-requirements.txt changes)
COPY pip-requirements.txt .
RUN pip install --break-system-packages -r pip-requirements.txt

# Copy source and compile
COPY . .
RUN bash compile.sh

RUN mkdir -p input output

RUN chown -R pdp:pdp ${TOOLDIR}
RUN chmod -R 755 ${TOOLDIR}

RUN echo 'PS1="\[\033[01;34m\]\u@\h:\[\033[01;32m\]\w\[\033[00m\]\$ "' >> /home/pdp/.bashrc && \
    echo 'alias ll="ls -lh --color=auto"' >> /home/pdp/.bashrc && \
    echo 'alias la="ls -lha --color=auto"' >> /home/pdp/.bashrc && \
    echo 'export LS_COLORS="di=01;34:ln=01;36:so=01;35:pi=33:ex=01;32:bd=40;33;01:cd=40;33;01"' >> /home/pdp/.bashrc

WORKDIR ${TOOLDIR}

USER pdp

ENTRYPOINT ["bash", "pandelosp.sh"]
