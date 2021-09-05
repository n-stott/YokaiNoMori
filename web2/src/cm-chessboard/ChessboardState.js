/**
 * Author and copyright: Stefan Haack (https://shaack.com)
 * Repository: https://github.com/shaack/cm-chessboard
 * License: MIT, see file 'LICENSE'
 */

export const REGION = {
    reserve0: "reserve0",
    reserve1: "reserve1",
    board: "board"
}

export class ChessboardState {

    constructor() {
        this.squares = new Array(12).fill(undefined)
        this.reserve0 = new Array(7).fill(undefined)
        this.reserve1 = new Array(7).fill(undefined)
        this.orientation = undefined
        this.markers = []
        this.res0markers = []
        this.res1markers = []
        this.inputWhiteEnabled = false
        this.inputBlackEnabled = false
        this.inputEnabled = false
        this.squareSelectEnabled = false
    }

    setPiece(index, piece) {
        this.squares[index] = piece
    }

    addMarker(index, type) {
        this.markers.push({index: index, type: type})
    }

    removeMarkers(index = undefined, type = undefined) {
        if (!index && !type) {
            this.markers = []
        } else {
            this.markers = this.markers.filter((marker) => {
                if (!marker.type) {
                    if (index === marker.index) {
                        return false
                    }
                } else if (!index) {
                    if (marker.type === type) {
                        return false
                    }
                } else if (marker.type === type && index === marker.index) {
                    return false
                }
                return true
            })
        }
    }

    setPosition(fen) {
        if (fen) {
            const parts = fen.split(/\//)
            for (let part = 0; part < 4; part++) {
                const row = parts[part]
                for (let c = 0; c < 3; c++) {
                    const char = row.substr(c, 1)
                    let piece = undefined
                    if (char !== '.') {
                        if (char.toUpperCase() === char) {
                            piece = `w${char.toLowerCase()}`
                        } else {
                            piece = `b${char}`
                        }
                    }
                    this.squares[part * 3 + c] = piece
                }
            }
            {
                const row = parts[4]
                for (let c = 0; c < 7; c++) {
                    const char = row.substr(c, 1)
                    let piece = undefined
                    if (char !== '.') {
                        if (char.toUpperCase() === char) {
                            piece = `w${char.toLowerCase()}`
                        } else {
                            piece = `b${char}`
                        }
                    }
                    this.reserve0[c] = piece
                }
            }
            {
                const row = parts[5]
                for (let c = 0; c < 7; c++) {
                    const char = row.substr(c, 1)
                    let piece = undefined
                    if (char !== '.') {
                        if (char.toUpperCase() === char) {
                            piece = `w${char.toLowerCase()}`
                        } else {
                            piece = `b${char}`
                        }
                    }
                    this.reserve1[c] = piece
                }
            }
        }
    }

    getPosition() {
        let parts = new Array(6).fill("")
        for (let part = 0; part < 4; part++) {
            for (let i = 0; i < 3; i++) {
                const piece = this.squares[part * 3 + i]
                if (!piece) {
                    parts[part] += "."
                } else {
                    const color = piece.substr(0, 1)
                    const name = piece.substr(1, 1)
                    if (color === "w") {
                        parts[part] += name.toUpperCase()
                    } else {
                        parts[part] += name
                    }
                }
            }
        }
        {
            for (let i = 0; i < 7; i++) {
                const piece = this.reserve0[i]
                if (!piece) {
                    parts[4] += "."
                } else {
                    const color = piece.substr(0, 1)
                    const name = piece.substr(1, 1)
                    if (color === "w") {
                        parts[4] += name.toUpperCase()
                    } else {
                        parts[4] += name
                    }
                }
            }
        }
        {
            for (let i = 0; i < 7; i++) {
                const piece = this.reserve1[i]
                if (!piece) {
                    parts[5] += "."
                } else {
                    const color = piece.substr(0, 1)
                    const name = piece.substr(1, 1)
                    if (color === "w") {
                        parts[5] += name.toUpperCase()
                    } else {
                        parts[5] += name
                    }
                }
            }
        }
        return parts.join("/")
    }

    squareToIndex(square) {
        const file = square.substr(0, 1).charCodeAt(0) - 97
        const rank = square.substr(1, 1) - 1
        return 3 * rank + file
    }

}