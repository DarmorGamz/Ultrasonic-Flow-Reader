
// 2. This code loads the IFrame Player API code asynchronously.
var tag = document.createElement('script');

tag.src = "https://www.youtube.com/iframe_api";
var firstScriptTag = document.getElementsByTagName('script')[0];
firstScriptTag.parentNode.insertBefore(tag, firstScriptTag);

var player;
function onYouTubeIframeAPIReady() {
    player = new YT.Player('player', {
        height: '0',
        width: '0',
        videoId: 'dQw4w9WgXcQ',
        playerVars: {
            'playsinline': 1,
            'loop': 1,
            'mute': 1,
            'autoplay': 0,
        },
        allow: "autoplay",
        events: {
            'onReady': onPlayerReady
        }
    });
}

function onPlayerReady() {
    var rickroll = document.getElementById('rickroll');
    rickroll.addEventListener('click', function() {
        if (player && player.isMuted()) {
            player.playVideo();
            player.unMute();
        } else {
            player.pauseVideo();
            player.mute();
        }
    });
}
