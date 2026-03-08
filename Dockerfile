FROM ubuntu:22.04 AS build

LABEL maintainer="Jorik Devreese"
LABEL version="0.1"
LABEL description="docker image compiling c code to armv5te."

RUN DEBIAN_FRONTEND=noninteractive apt-get update -y &&  \
    DEBIAN_FRONTEND=noninteractive apt-get upgrade -y &&  \
    DEBIAN_FRONTEND=noninteractive apt-get install --no-install-recommends -y build-essential git libpng-dev gdebi-core python3 python3-pip cmake automake libnewlib-arm-none-eabi libnewlib-dev libstdc++-arm-none-eabi-dev libstdc++-arm-none-eabi-newlib gcc-arm-none-eabi mono-devel bsdmainutils && \
    DEBIAN_FRONTEND=noninteractive apt-get update -y

COPY dependencies /tmp
RUN pip install -r /tmp/requirements.txt && rm /tmp/requirements.txt

COPY entrypoint.sh /usr/local/bin/entrypoint.sh
RUN chmod +x /usr/local/bin/entrypoint.sh
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
CMD ["/bin/bash"]
